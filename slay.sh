#!/usr/bin/env bash
# copy_all_texts.sh
# Usage: ./copy_all_texts.sh [DIR] [-r]
# Default DIR = . (répertoire courant). Option -r pour récursif.

set -euo pipefail

DIR="."
RECURSIVE=0

# parse args simples
while [[ $# -gt 0 ]]; do
  case "$1" in
    -r|--recursive) RECURSIVE=1; shift ;;
    -h|--help)
      cat <<EOF
Usage: $0 [DIR] [-r]
Affiche et copie dans le presse-papiers le contenu de tous les fichiers texte d'un répertoire.
- DIR : répertoire (par défaut .)
- -r  : parcourir récursivement
EOF
      exit 0
      ;;
    *)
      DIR="$1"; shift
      ;;
  esac
done

if [[ ! -d "$DIR" ]]; then
  echo "Erreur : $DIR n'est pas un répertoire." >&2
  exit 2
fi

# detect clipboard tool
CLIP_CMD=""
if command -v wl-copy >/dev/null 2>&1; then
  CLIP_CMD="wl-copy"
elif command -v xclip >/dev/null 2>&1; then
  CLIP_CMD="xclip -selection clipboard"
elif command -v xsel >/dev/null 2>&1; then
  CLIP_CMD="xsel --clipboard --input"
else
  echo "Aucun utilitaire de presse-papiers trouvé (wl-copy, xclip ou xsel). Installe en un pour activer la copie." >&2
  CLIP_CMD=""
fi

TMP=$(mktemp)
trap 'rm -f "$TMP"' EXIT

# build find command
if [[ $RECURSIVE -eq 1 ]]; then
  FIND_CMD=(find "$DIR" -type f)
else
  FIND_CMD=(find "$DIR" -maxdepth 1 -type f)
fi

# iterate files
echo "Parcours des fichiers dans '$DIR' (récursif=$RECURSIVE)..."
count=0
while IFS= read -r -d '' file; do
  # detecter si c'est un fichier texte via file --mime-type si disponible
  is_text=1
  if command -v file >/dev/null 2>&1; then
    mime=$(file --mime-type -b -- "$file")
    # considérer texte si mime commence par text/ ou certains application/* connus textuels
    if [[ $mime == text/* ]] || [[ $mime =~ ^application/(json|xml|javascript|x-.*|.*\+xml)$ ]]; then
      is_text=0  # 0 signifie vrai pour notre test suivant
    else
      is_text=1
    fi
  else
    # fallback heuristique : grep -Iq (ne produit pas de sortie)
    if grep -Iq . -- "$file" 2>/dev/null; then
      is_text=0
    else
      is_text=1
    fi
  fi

  if [[ $is_text -eq 0 ]]; then
    echo "=== $file ==="
    # afficher avec un séparateur
    echo "=== $file ===" >> "$TMP"
    # cat en mode safe
    if ! cat -- "$file"; then
      echo "Erreur de lecture sur $file" >&2
    fi
    echo # nouvelle ligne après affichage
    cat -- "$file" >> "$TMP"
    echo -e "\n" >> "$TMP"
    ((count++))
  else
    echo "Ignoré (binaire ou non-texte): $file"
  fi
done < <("${FIND_CMD[@]}" -print0)

echo
echo "Fichiers texte traités : $count"

if [[ -n "$CLIP_CMD" ]]; then
  # copier dans le presse-papiers
  if [[ -s "$TMP" ]]; then
    # shellcheck disable=SC2086
    eval "$CLIP_CMD" < "$TMP"
    echo "Le contenu a été copié dans le presse-papiers."
  else
    echo "Rien à copier (pas de contenu texte trouvé)."
  fi
else
  echo "Aucun utilitaire clipboard disponible : le contenu n'a pas été copié."
  echo "Contenu rassemblé dans : $TMP (fichier temporaire, supprimé à la sortie)."
fi
