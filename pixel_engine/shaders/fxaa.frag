#version 430

// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html

in vec2 v_uv;

uniform sampler2D in_image;
uniform int width;
uniform int height;

out vec4 f_color;

float kEdgeThresholdMin = 0.0312;
float kEdgeThresholdMax = 0.125;
int kIterations = 12;
float kQuality[12] = {1, 1, 1, 1, 1, 1.5, 2, 2, 2, 2, 4, 8};
float kSubpixelQuality = .75;

float rgb2luma() {
    return 0;
}

float luma(vec2 pixel_coordinate, ivec2 offset) {
    vec4 rgba = texture(in_image, pixel_coordinate + (offset * vec2(1/float(width),1/float(height))));
    return sqrt(dot(rgba.rgb, vec3(0.299, 0.587, 0.114)));
}

void main() {
    vec2 pixel_coordinate = v_uv;

    // Compute this pixel luma
    float luma_center = luma(pixel_coordinate, ivec2(0, 0));

    // Compute edge lumas
    float luma_up = luma(pixel_coordinate, ivec2(0, 1));
    float luma_down = luma(pixel_coordinate, ivec2(0, -1));
    float luma_left = luma(pixel_coordinate, ivec2(-1, 0));
    float luma_right = luma(pixel_coordinate, ivec2(1, 0));

    // Compute min/max
    float luma_min = min(luma_center, min(min(luma_up, luma_down), min(luma_left, luma_right)));
    float luma_max = max(luma_center, max(max(luma_up, luma_down), max(luma_left, luma_right)));

    // Compute range
    float luma_range = luma_max - luma_min;

    // If range doesn't make threshold, no anti-aliasing
    if (luma_range < max(kEdgeThresholdMin, luma_max * kEdgeThresholdMax)) {
        f_color = texture(in_image, pixel_coordinate);
        return;
    }

    // Compute corner lumas
    float luma_up_left = luma(pixel_coordinate, ivec2(-1, 1));
    float luma_down_left = luma(pixel_coordinate, ivec2(-1, -1));
    float luma_up_right = luma(pixel_coordinate, ivec2(1, 1));
    float luma_down_right = luma(pixel_coordinate, ivec2(1, -1));

    // Combine the four edges lumas (using intermediary variables for future computations with the same values).
    float luma_down_up = luma_down + luma_up;
    float luma_left_right = luma_left + luma_right;

    // Combine corner values
    float luma_left_corners = luma_up_left + luma_down_left;
    float luma_down_corners = luma_down_left + luma_down_right;
    float luma_up_corners = luma_up_left + luma_up_right;
    float luma_right_corners = luma_up_right + luma_down_right;

    // Compute an estimation of the gradient along the horizontal and vertical axis.
    float edge_horizontal =  abs(-2.0 * luma_left + luma_left_corners)  + abs(-2.0 * luma_center + luma_down_up ) * 2.0 + abs(-2.0 * luma_right + luma_right_corners);
    float edge_vertical = abs(-2.0 * luma_up + luma_up_corners) + abs(-2.0 * luma_center + luma_left_right) * 2.0  + abs(-2.0 * luma_down + luma_down_corners);

    // Is the local edge horizontal or vertical ?
    bool is_horizontal = (edge_horizontal >= edge_vertical);

    // Select the two neighboring texels lumas in the opposite direction to the local edge.
    float luma1 = is_horizontal ? luma_down : luma_left;
    float luma2 = is_horizontal ? luma_up : luma_right;
    // Compute gradients in this direction.
    float gradient1 = luma1 - luma_center;
    float gradient2 = luma2 - luma_center;

    // Which direction is the steepest ?
    bool is_1_steepest = abs(gradient1) >= abs(gradient2);

    // Gradient in the corresponding direction, normalized.
    float gradient_scaled = 0.25*max(abs(gradient1),abs(gradient2));

    // Choose the step size (one pixel) according to the edge direction.
    float step_length = is_horizontal ? 1 / float(height) : 1 / float(width);

    // Average luma in the correct direction.
    float luma_local_average = 0.0;

    if(is_1_steepest){
        // Switch the direction
        step_length = -step_length;
        luma_local_average = 0.5*(luma1 + luma_center);
    } else {
        luma_local_average = 0.5*(luma2 + luma_center);
    }

    // Shift UV in the correct direction by half a pixel.
    vec2 current_pixel_coordinate = pixel_coordinate;
    if(is_horizontal){
        current_pixel_coordinate.y += step_length * 0.5;
    } else {
        current_pixel_coordinate.x += step_length * 0.5;
    }

    // Compute offset (for each iteration step) in the right direction.
    vec2 offset = is_horizontal ? vec2(1 / float(width), 0) : vec2(0, 1 / float(height));
    // Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
    vec2 pixel_coordinate1 = current_pixel_coordinate - offset;
    vec2 pixel_coordinate2 = current_pixel_coordinate + offset;

    // Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
    float luma_end1 = luma(pixel_coordinate1, ivec2(0, 0));
    float luma_end2 = luma(pixel_coordinate2, ivec2(0, 0));
    luma_end1 -= luma_local_average;
    luma_end2 -= luma_local_average;

    // If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
    bool reached1 = abs(luma_end1) >= gradient_scaled;
    bool reached2 = abs(luma_end2) >= gradient_scaled;
    bool reached_both = reached1 && reached2;

    // If the side is not reached, we continue to explore in this direction.
    if(!reached1){
        pixel_coordinate1 -= offset;
    }
    if(!reached2){
        pixel_coordinate2 += offset;
    }

    // If both sides have not been reached, continue to explore.
    if(!reached_both){

        for(int i = 2; i < kIterations; i++){
            // If needed, read luma in 1st direction, compute delta.
            if(!reached1){
                luma_end1 = luma(pixel_coordinate1, ivec2(0, 0));
                luma_end1 = luma_end1 - luma_local_average;
            }
            // If needed, read luma in opposite direction, compute delta.
            if(!reached2){
                luma_end2 = luma(pixel_coordinate2, ivec2(0, 0));
                luma_end2 = luma_end2 - luma_local_average;
            }
            // If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
            reached1 = abs(luma_end1) >= gradient_scaled;
            reached2 = abs(luma_end2) >= gradient_scaled;
            reached_both = reached1 && reached2;

            // If the side is not reached, we continue to explore in this direction, with a variable quality.
            if(!reached1){
                pixel_coordinate1 -= offset * kQuality[i];
            }
            if(!reached2){
                pixel_coordinate2 += offset * kQuality[i];
            }

            // If both sides have been reached, stop the exploration.
            if(reached_both){ break; }
        }
    }

    // Compute the distances to each extremity of the edge.
    float distance1 = is_horizontal ? (v_uv.x - pixel_coordinate1.x) : (v_uv.y - pixel_coordinate1.y);
    float distance2 = is_horizontal ? (pixel_coordinate2.x - v_uv.x) : (pixel_coordinate2.y - v_uv.y);

    // In which direction is the extremity of the edge closer ?
    bool is_direction1 = distance1 < distance2;
    float distance_final = min(distance1, distance2);

    // Length of the edge.
    float edge_thickness = (distance1 + distance2);

    // UV offset: read in the direction of the closest side of the edge.
    float pixel_offset = -distance_final / edge_thickness + 0.5;

    // Is the luma at center smaller than the local average ?
    bool is_luma_center_smaller = luma_center < luma_local_average;

    // If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
    // (in the direction of the closer side of the edge.)
    bool correct_variation = ((is_direction1 ? luma_end1 : luma_end2) < 0.0) != is_luma_center_smaller;

    // If the luma variation is incorrect, do not offset.
    float final_offset = correct_variation ? pixel_offset : 0.0;

    // Sub-pixel shifting
    // Full weighted average of the luma over the 3x3 neighborhood.
    float luma_average = (1.0/12.0) * (2.0 * (luma_down_up + luma_left_right) + luma_left_corners + luma_right_corners);
    // Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
    float sub_pixel_offset1 = clamp(abs(luma_average - luma_center)/luma_range,0.0,1.0);
    float sub_pixel_offset2 = (-2.0 * sub_pixel_offset1 + 3.0) * sub_pixel_offset1 * sub_pixel_offset1;
    // Compute a sub-pixel offset based on this delta.
    float sub_pixel_offset_final = sub_pixel_offset2 * sub_pixel_offset2 * kSubpixelQuality;

    // Pick the biggest of the two offsets.
    final_offset = max(final_offset, sub_pixel_offset_final);

    // Compute the final UV coordinates.
    vec2 final_pixel_coordinate = v_uv;
    if(is_horizontal) {
        final_pixel_coordinate.y += final_offset * step_length;
    } else {
        final_pixel_coordinate.x += final_offset * step_length;
    }

    // Read the color at the new UV coordinates, and use it.
    f_color = texture(in_image, final_pixel_coordinate);
}