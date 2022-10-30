# ROM integrity check

- [ROM integrity check](#rom-integrity-check)
  - [Generate a sfc_md5sum.ref for a new release](#generate-a-sfc_md5sumref-for-a-new-release)
  - [ROM integrity check](#rom-integrity-check-1)
    - [From Docker](#from-docker)
    - [Manually](#manually)
  - [TODO](#todo)

This page is supposed to briefly explain the integrity check of roms after a new build. This procedure consists of comparing the hash of the generated rom with the reference one.

## Generate a sfc_md5sum.ref for a new release

After each new release, this file must be updated.

Position yourself in the stable branch (master) and run the following command:

```bash
find snes-examples -not \( -path snes-examples/bin -prune \) \
    -type f -name "*.sfc" -print0 | \
    xargs -0 md5sum | awk '{print $1"|"$2}' > "${PVSNESLIB_HOME}/sfc_md5sum.ref"
```

## ROM integrity check

Create your working or test branch, make your code changes. then you can test if the ROMs (SFC file) have changed.

How to do it ?

### From Docker

This control is already included during the execution of the `dockerrun.sh` script, so there is nothing specific to do.

### Manually

Just set PVSNESHOME_LIB, source the library as follow and run the make command at the root of the repository.

```bash
export PVSNESHOME_LIB=$(pwd)
source docker/scripts/include/libraries.sh
make
```

Then execute the function

```bash
f_check_sfc_md5
```

The function will:

- Fail if a rom listed in sfc_md5sum.ref is missing from your testing branch after the build process (make). This means that you introduced a regression with your changes. So let's fix that.
- Send a warning if the md5 hash of the SFC file has changed. In this case, it is strongly recommended to test the ROM with an emulator to check if there is any regression. If all of your tests are good, you can consider it a false positive.


## TODO
