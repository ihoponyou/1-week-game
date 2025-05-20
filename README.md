# one week game

## building

with terminal:
```
mkdir -p build
cd build
cmake ..
cmake --build ./build --config <Debug|Release> --target game -j 10 --
```

with bash:
```
sh build.sh <Debug|Release>
```

with just:
```
just build-<debug|release>
```

## playing

with terminal:
```
./build/game.exe
```

with just:
```
just play
```
