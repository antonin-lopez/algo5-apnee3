#!/bin/env bash

SOURCE_DIR='./tests/source-files/'
ENCODED_DIR='./tests/encoded-files/'
DECODED_DIR='./tests/decoded-files/'

SOURCE_FILE_FORMAT='.txt'
ENCODED_FILE_FORMAT='_encoded.hf'
DECODED_FILE_FORMAT='_decoded.txt'

FILE_NAME='test'

echo "--- $FILE_NAME ---"

./huff_encode "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT" "$ENCODED_DIR/$FILE_NAME$ENCODED_FILE_FORMAT" > /dev/null 2>&1
./huff_decode "$ENCODED_DIR/$FILE_NAME$ENCODED_FILE_FORMAT" "$DECODED_DIR/$FILE_NAME$DECODED_FILE_FORMAT" > /dev/null 2>&1

if diff "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT" "$DECODED_DIR/$FILE_NAME$DECODED_FILE_FORMAT" > /dev/null; then
  echo "Le fichier décodé est identique au fichier source"
else
  echo "Le fichier décodé est différent du fichier source"
fi

