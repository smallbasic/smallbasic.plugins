#/bin/bash

function patch_aar() {
    # Create temporary directory
    # Extract AAR contents
    # Extract classes.jar
    # Copy extra classes
    # Repackage classes.jar
    # Repackage AAR
    # Clean up temporary directory
    rm -rf "${TEMP_DIR}" && \
    mkdir -p "${TEMP_DIR}" && \
    unzip "build/outputs/aar/${AAR_FILE}" -d "${TEMP_DIR}" && \
    mkdir -p "${CLASSES_DIR}" && \
    unzip "${CLASSES_JAR}" -d "${CLASSES_DIR}" && \
    rm "${CLASSES_JAR}" && \
    cp -r "${DEPENDENCY_CLASSES_DIR}"/ioio* "${CLASSES_DIR}" && \
    (cd "${CLASSES_DIR}" && jar cvf ../classes.jar .) && \
    rm -rf "${CLASSES_DIR}" && \
    (cd "${TEMP_DIR}" && zip -r "../${AAR_FILE}" *) && \
    rm -rf "${TEMP_DIR}"
}

TEMP_DIR="patch"
DEPENDENCY_CLASSES_DIR="lib"
CLASSES_DIR="${TEMP_DIR}/classes"
CLASSES_JAR="${TEMP_DIR}/classes.jar"

AAR_FILE="ioio-debug.aar"
patch_aar

AAR_FILE="ioio-release.aar"
patch_aar
