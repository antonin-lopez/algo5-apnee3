#!/bin/env bash

SOURCE_DIR='./tests/source-files/'
ENCODED_DIR='./tests/encoded-files/'
DECODED_DIR='./tests/decoded-files/'

SOURCE_FILE_FORMAT='.txt'
ENCODED_FILE_FORMAT='_encoded.hf'
DECODED_FILE_FORMAT='_decoded.txt'

FILE_NAME='test'

for fichier in "$SOURCE_DIR"*"$SOURCE_FILE_FORMAT"; do
  FILE_NAME=$(basename "$fichier" "$SOURCE_FILE_FORMAT")

  echo "--- $FILE_NAME ---"

  echo "Nombre de caractères dans le fichier source : $(wc -c < "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT")"

  nb_distinct=$(awk '
  {
    for (i=1; i<=length($0); i++) {
      c = substr($0, i, 1)
      chars[c] = 1
    }
  }
  END { print length(chars) }
  ' "$fichier")

  echo "Nombre de caractères distincts : $nb_distinct"

  ./huff_encode "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT" "$ENCODED_DIR/$FILE_NAME$ENCODED_FILE_FORMAT" > /dev/null 2>&1
  ./huff_decode "$ENCODED_DIR/$FILE_NAME$ENCODED_FILE_FORMAT" "$DECODED_DIR/$FILE_NAME$DECODED_FILE_FORMAT" > /dev/null 2>&1

  taille_source=$(stat -c%s "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT")
  taille_encode=$(stat -c%s "$ENCODED_DIR/$FILE_NAME$ENCODED_FILE_FORMAT")

  if [[ "$taille_source" -eq 0 ]]; then
      echo "Erreur : taille du fichier source = 0"
      exit 1
  fi

  taux=$(echo "scale=2; $taille_encode * 100 / $taille_source" | bc)
  echo "Taux de compression : $taux%"

  if diff "$SOURCE_DIR/$FILE_NAME$SOURCE_FILE_FORMAT" "$DECODED_DIR/$FILE_NAME$DECODED_FILE_FORMAT" > /dev/null; then
    echo "Le fichier décodé est identique au fichier source"
  else
    echo "Le fichier décodé est différent du fichier source"
  fi

  echo ""
done

