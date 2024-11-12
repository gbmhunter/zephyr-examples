## Install Dependencies

Clone this repository and `cd` into the root directory.

Create a Python virtual environment and install `west`:

```shell
python3 -m venv .venv
source .venv/bin/activate
pip install west
```

Install additional Python dependencies:

```shell
pip install -r zephyr/scripts/requirements.txt
```

Run `west update` to install Zephyr dependencies:

```shell
west update
```

## Build and Run

To build a new project for the first time:

```shell
west build -b native_sim ./apps/rgb-led-with-tests/
```

Run (and re-build):

```
west build -t run
```

To export compile commands for Intellisense, this was run:

```shell
west config build.cmake-args -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

To run tests for an app which contains them (in a `test/` directory), run:

```shell
west twister -T apps/<app_directory>/tests/
```

For example:

```shell
west twister -T apps/cpp-app-framework/tests/
```
