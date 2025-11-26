#!/bin/bash

# Forcer la locale pour les nombres avec point décimal
export LC_NUMERIC=C

SOURCE_DIR='./tests/source/'
ENCODED_DIR='./tests/encoded/'
DECODED_DIR='./tests/decoded/'

# Tableau associatif pour stocker les sommes et compteurs
declare -A sommes
declare -A compteurs
declare -A stats_construction_table
declare -A stats_construction_arbre
declare -A stats_encodage

echo "=== Analyse de compression Huffman ==="
echo ""

# Vérifier que les dossiers existent
if [ ! -d "$SOURCE_DIR" ] || [ -z "$(ls -A $SOURCE_DIR)" ]; then
    echo "Erreur: Le dossier source est vide ou n'existe pas."
    echo "Veuillez d'abord exécuter: ./test_generator"
    exit 1
fi

echo "=== Début des tests ==="
echo ""

total_tests=0
tests_reussis=0

# Parcourir tous les fichiers de test
for fichier in "$SOURCE_DIR"*.txt; do
    if [ ! -f "$fichier" ]; then
        continue
    fi
    
    FILE_NAME=$(basename "$fichier" .txt)
    
    # Extraire les paramètres du nom de fichier (format: test_nXXX_dYY_rZ.txt)
    if [[ $FILE_NAME =~ test_n([0-9]+)_d([0-9]+)_r([0-9]+) ]]; then
        NB_CHARS="${BASH_REMATCH[1]}"
        NB_DISTINCT="${BASH_REMATCH[2]}"
        REP="${BASH_REMATCH[3]}"
    else
        continue
    fi
    
    total_tests=$((total_tests + 1))
    
    # Afficher la progression tous les 50 tests
    if [ $((total_tests % 50)) -eq 0 ]; then
        echo "Progression: $total_tests tests effectués..."
    fi

     # Clé pour le tableau associatif
    cle="${NB_CHARS}_${NB_DISTINCT}"
    
    # Encodage (silencieux)
    output_encodage=$(./huff_encode "$fichier" "$ENCODED_DIR/${FILE_NAME}_encoded.hf") #> /dev/null 2>&1
    if [ $? -ne 0 ]; then
        printf "Erreur lors de l'encodage du fichier %s\n" "$fichier"
        continue
    fi
    
    # Décodage (silencieux)
    ./huff_decode "$ENCODED_DIR/${FILE_NAME}_encoded.hf" "$DECODED_DIR/${FILE_NAME}_decoded.txt" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        continue
    fi
    
    # Calcul des tailles
    taille_source=$(stat -c%s "$fichier" 2>/dev/null || stat -f%z "$fichier" 2>/dev/null)
    taille_encodee=$(stat -c%s "$ENCODED_DIR/${FILE_NAME}_encoded.hf" 2>/dev/null || stat -f%z "$ENCODED_DIR/${FILE_NAME}_encoded.hf" 2>/dev/null)
    
    if [[ "$taille_source" -eq 0 ]]; then
        continue
    fi
    
    # Vérification de l'intégrité
    if ! diff "$fichier" "$DECODED_DIR/${FILE_NAME}_decoded.txt" > /dev/null 2>&1; then
        continue
    fi
    
    tests_reussis=$((tests_reussis + 1))
    
    # Calcul du taux de compression
    taux=$(echo "scale=4; $taille_encodee * 100 / $taille_source" | bc)
    
    
    # Accumuler les sommes
    if [ -z "${sommes[$cle]}" ]; then
        sommes[$cle]="0"
        compteurs[$cle]="0"
    fi

    IFS=':' read -r tmp_table tmp_arbre tmp_encodage <<< "$output_encodage"

    if [[ -z "$tmp_table" || -z "$tmp_arbre" || -z "$tmp_encodage" ]]; then
        echo "WARNING: sortie encodage invalide : «$output_encodage»"
        continue
    else
        stats_construction_table[$cle]=$(echo "${stats_construction_table[$cle]:-0} + $tmp_table" | bc)
        stats_construction_arbre[$cle]=$(echo "${stats_construction_arbre[$cle]:-0} + $tmp_arbre" | bc)
        stats_encodage[$cle]=$(echo "${stats_encodage[$cle]:-0} + $tmp_encodage" | bc)
    fi  

    sommes[$cle]=$(echo "${sommes[$cle]} + $taux" | bc)
    compteurs[$cle]=$((compteurs[$cle] + 1))
done

echo ""
echo "=== Tests terminés ==="
echo "Total: $total_tests tests effectués, $tests_reussis réussis"
echo ""
echo "========================================================================================================"
echo "  Taille  | Distincts | Nb tests | Taux compression moyen | Temps Table | Temps Arbre | Temps Encodage"
echo "========================================================================================================"

# Trier et afficher les résultats
for taille in 100 500 1000 5000 10000 50000; do
    for distinct in 2 5 10 20 50 100; do
        cle="${taille}_${distinct}"

        if [ -n "${compteurs[$cle]}" ] && [ "${compteurs[$cle]}" -gt 0 ]; then
            moyenne_table=$(echo "scale=4; ${stats_construction_table[$cle]} / ${compteurs[$cle]}" | bc)
            moyenne_arbre=$(echo "scale=4; ${stats_construction_arbre[$cle]} / ${compteurs[$cle]}" | bc)
            moyenne_encodage=$(echo "scale=4; ${stats_encodage[$cle]} / ${compteurs[$cle]}" | bc)
            moyenne=$(echo "scale=2; ${sommes[$cle]} / ${compteurs[$cle]}" | bc)
            printf " %8d | %9d | %8d | %6.2f%% | %10.5f | %10.5f | %10.5f\n" \
            "$taille" "$distinct" "${compteurs[$cle]}" \
            "$moyenne" "$moyenne_table" "$moyenne_arbre" "$moyenne_encodage"

        fi
    done
    echo "-----------------------------------------------------------------------------------"
done

echo ""
echo "Note: Le taux de compression représente (taille_encodée / taille_source) × 100"
echo "      Un taux plus bas = meilleure compression"