#acord persiune de citire deoare stiu sigur ca nu avem nici o permisiune
  chmod +r "$1"
# Modelul pe care dorim să îl căutăm
pattern="cauta"

# Verifică dacă modelul este prezent în fișier
if grep -q "$pattern" "$1"; then
  # Dacă modelul este găsit, returnează 1
  result="rau"
else
  # Dacă modelul nu este găsit, returnează 0
  result="bun"
fi
echo $result
