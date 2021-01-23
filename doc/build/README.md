# Build Instructions

After cloning you must follow some instructions to be able to compile your project with algine.

## Dependencies

Python 3 is required. Make sure your python version is higher than 3.
```bash
python --version
# or, if previous displays 2.x
python3 --version
```

## Steps

1. `cd contrib`
2. Download one-header libraries: `python contrib.py`
3.  ```bash
    cd glew
    make extensions
    ```
4. Follow additional steps:
    * [Debian](Debian.md)
    * [Windows](Windows.md)
