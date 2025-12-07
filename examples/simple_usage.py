import numpy as np

import svd


def main() -> None:
    matrix = np.array(
        [
            [3.0, 1.0, 1.0],
            [-1.0, 3.0, 1.0],
            [0.0, 0.0, 2.0],
        ],
        dtype=np.float64,
    )

    u, s, v = svd.svd(matrix)

    print("Matrix:\n", matrix)
    print("U:\n", u)
    print("Singular values:", s)
    print("V:\n", v)


if __name__ == "__main__":
    main()

