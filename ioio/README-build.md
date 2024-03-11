## Building the desktop jar

```
cd ioio
mvn clean package
```

The resulting jar can be found in `ioio/target`

## Building the android aar

```
./gradlew clean assemble
```

Copy the resulting aar files to SmallBASIC project

```
cp ioio/build/outputs/aar/* ~/src/SmallBASIC/src/platform/android/app/libs/
```

### To setup the gradlew command

1. Download an install gradle per gradle instructions
2. Setup the wrapper

```
gradle wrapper
```
