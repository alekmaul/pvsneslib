name: PVSNESLIB Build & Release

on:
  # workflow_dispatch:
  push:
    branches:
      - "pvsneslib_runners"

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        include:
          - os: ubuntu-latest
            name: ubuntu-latest
          - os: windows-latest
            name: windows-latest
          - os: macos-latest
            name: macos-latest

    steps:
      # ------------------------------------ #
      # Install dependencies                 #
      # ------------------------------------ #
      - name: Install dependencies for Linux
        if: matrix.name == 'ubuntu-latest'
        run: |
          sudo apt-get install -y build-essential gcc-12 cmake make git doxygen
          sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 90
          cmake --version
          gcc -v
          make -v
      - name: Install dependencies for Windows
        if: matrix.name == 'windows-latest'
        uses: msys2/setup-msys2@v2
        with:
          update: true
          install: >-
            base-devel
            gcc
            make
            cmake
            pcre-devel
            git
            python
            doxygen
      - name: Install dependencies for MacOS
        if: matrix.name == 'macos-latest'
        run: |
          brew install pcre
          brew install doxygen
          brew install gnu-sed

      # ------------------------------------ #
      # Checkout the project                 #
      # ------------------------------------ #
      - name: Check out PVSNESLIB
        uses: actions/checkout@v3
        with:
          ref: ${{ github.head_ref }}
          path: pvsneslib
          fetch-depth: 0
          submodules: recursive
          persist-credentials: false

      # ------------------------------------ #
      # Compiling sources                    #
      # ------------------------------------ #
      - name: Build PVSNESLIB for Linux
        if: (matrix.name == 'ubuntu-latest')
        run: |
          cd pvsneslib
          export PVSNESLIB_HOME=$(pwd)
          make
      - name: Build PVSNESLIB for Windows
        if: (matrix.name == 'windows-latest')
        shell: msys2 {0}
        run: |
          cd pvsneslib
          export PVSNESLIB_HOME=$(pwd)
          make
      - name: Build PVSNESLIB for MacOS
        if: (matrix.name == 'macos-latest')
        run: |
          cd pvsneslib
          export PVSNESLIB_HOME=$(pwd)
          PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
          sudo ln -s /usr/local/bin/gcc-12 /usr/local/bin/gcc
          sudo ln -s /usr/local/bin/g++-12 /usr/local/bin/g++
          CFLAGS+=-I/usr/local/include/ make

      # ------------------------------------ #
      # Compiling sources and create release #
      # ------------------------------------ #
      # - name: Build & Release PVSNESLIB for Linux
      #   if: (matrix.name == 'ubuntu-latest')
      #   run: |
      #     cd pvsneslib
      #     export PVSNESLIB_HOME=$(pwd)
      #     make release
      # - name: Build & Release PVSNESLIB for Windows
      #   if: (matrix.name == 'windows-latest')
      #   run: |
      #     cd pvsneslib
      #     export PVSNESLIB_HOME=$(pwd)
      #     make release
      # - name: Build & Release PVSNESLIB for MacOS
      #   if: (matrix.name == 'macos-latest')
      #   run: |
      #     cd pvsneslib
      #     export PVSNESLIB_HOME=$(pwd)
      #     make release

      # ------------------------------------ #
      # Upload release                       #
      # ------------------------------------ #
      # - name: Upload PVSNESLIB release for Linux
      #   if: (matrix.name == 'ubuntu-latest')
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: pvsneslib-linux.zip
      #     path: pvnseslib/release/pvsneslib-linux.zip
      # - name: Upload PVSNESLIB release for Windows
      #   if: (matrix.name == 'windows-latest')
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: pvsneslib-windows.zip
      #     path: pvnseslib/release/pvsneslib-windows.zip
      # - name: Upload PVSNESLIB release for MacOS
      #   if: (matrix.name == 'macos-latest')
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: pvsneslib-darwin.zip
      #     path: pvnseslib/release/pvsneslib-darwin.zip

