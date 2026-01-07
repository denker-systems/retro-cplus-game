---
description: GitHub Actions CI/CD pipeline configuration
---

# GitHub Actions CI/CD

> Automated build, test, and deployment pipeline

## Basic CI Workflow

Create `.github/workflows/ci.yml`:

```yaml
name: CI Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [windows-latest]
        build_type: [Debug, Release]

    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Setup vcpkg
        uses: lukka/run-vcpkg@v11
        with:
          vcpkgGitCommitId: '2024.01.12'

      - name: Configure CMake
        run: |
          cmake -B build \
            -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
            -DCMAKE_TOOLCHAIN_FILE=${{ github.workspace }}/vcpkg/scripts/buildsystems/vcpkg.cmake

      - name: Build
        run: cmake --build build --config ${{ matrix.build_type }} --parallel

      - name: Test
        run: ctest --test-dir build -C ${{ matrix.build_type }} --output-on-failure

      - name: Upload Artifacts
        if: matrix.build_type == 'Release'
        uses: actions/upload-artifact@v4
        with:
          name: build-${{ matrix.os }}
          path: build/Release/
```

---

## Matrix Build (Multi-Platform)

```yaml
jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      fail-fast: false
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
        build_type: [Debug, Release]
        include:
          - os: windows-latest
            triplet: x64-windows
          - os: ubuntu-latest
            triplet: x64-linux
          - os: macos-latest
            triplet: x64-osx
```

---

## Code Quality Checks

```yaml
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Run clang-format
        uses: jidicula/clang-format-action@v4
        with:
          clang-format-version: '17'
          check-path: 'src'
      
      - name: Run cppcheck
        run: |
          sudo apt-get install cppcheck
          cppcheck --enable=all --error-exitcode=1 src/
```

---

## Release Workflow

Create `.github/workflows/release.yml`:

```yaml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup vcpkg
        uses: lukka/run-vcpkg@v11
      
      - name: Build Release
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Release
          cmake --build build --config Release
      
      - name: Package
        run: |
          mkdir dist
          cp build/Release/*.exe dist/
          cp -r assets dist/assets
      
      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            dist/*
          generate_release_notes: true
```

---

## Secrets Management

```yaml
# Use GitHub Secrets for sensitive data
- name: Deploy
  env:
    API_KEY: ${{ secrets.DEPLOY_API_KEY }}
    SERVER: ${{ secrets.DEPLOY_SERVER }}
  run: ./deploy.sh
```

**Required Secrets:**
- `DEPLOY_API_KEY` - Deployment API key
- `CODECOV_TOKEN` - Code coverage upload
- `DISCORD_WEBHOOK` - Notifications

---

## Caching

```yaml
- name: Cache vcpkg
  uses: actions/cache@v4
  with:
    path: |
      ${{ github.workspace }}/vcpkg
      !${{ github.workspace }}/vcpkg/buildtrees
    key: vcpkg-${{ matrix.triplet }}-${{ hashFiles('vcpkg.json') }}
    restore-keys: |
      vcpkg-${{ matrix.triplet }}-

- name: Cache CMake build
  uses: actions/cache@v4
  with:
    path: build
    key: build-${{ matrix.os }}-${{ matrix.build_type }}-${{ hashFiles('CMakeLists.txt') }}
```

---

## Status Badges

Add to README.md:
```markdown
![CI](https://github.com/[owner]/[repo]/actions/workflows/ci.yml/badge.svg)
![Release](https://github.com/[owner]/[repo]/actions/workflows/release.yml/badge.svg)
```

---

## Best Practices

- ✓ Use matrix builds for cross-platform
- ✓ Cache dependencies (vcpkg, build)
- ✓ Fail fast disabled for matrix
- ✓ Upload artifacts for debugging
- ✓ Use secrets for sensitive data
- ✓ Reuse workflows with `workflow_call`
- ✓ Pin action versions for reproducibility
