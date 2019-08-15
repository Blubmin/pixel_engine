#include <Eigen/Geometry>

namespace Eigen {
typedef AlignedBox<float, 2> Rectf;

template <typename T>
using Vector2 = Matrix<T, 2, 1>;

template <typename T>
using Vector3 = Matrix<T, 3, 1>;

template <typename T>
using Vector4 = Matrix<T, 4, 1>;

template <typename T>
using Isometry3 = Transform<T, 3, Isometry>;
}  // namespace Eigen