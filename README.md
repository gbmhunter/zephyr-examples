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