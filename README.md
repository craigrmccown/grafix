## Setup and Development
```bash
# Clone repo and submodules
git clone --recurse-submodules git@github.com:craigrmccown/grafix.git
cd grafix

# Generate build files
cmake -B build

# Build project
cmake --build build

# Run tests
ctest --test-dir build
```
