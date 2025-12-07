# Build stage: create wheel using python 3.10 slim
FROM python:3.10-slim AS builder

WORKDIR /app

# Install build dependencies for compiling pybind11 extension
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build && \
    rm -rf /var/lib/apt/lists/*

COPY . /app

# Build wheel using scikit-build-core
RUN python -m pip install --upgrade pip build && \
    python -m build

# Runtime stage: install the built wheel and verify it works without extra deps
FROM python:3.10-slim AS runtime

WORKDIR /runtime

COPY --from=builder /app/dist /wheel
COPY --from=builder /app/examples/simple_usage.py /runtime/examples/simple_usage.py

RUN python -m pip install --no-cache-dir /wheel/*.whl

CMD ["python", "./examples/simple_usage.py"]

# Export stage: used with `docker build --target wheel_export -o out_dir .`
FROM scratch AS wheel_export
COPY --from=builder /app/dist /

# Final stage (default image) references runtime environment
FROM runtime
