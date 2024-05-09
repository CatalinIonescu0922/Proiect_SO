if [ ! -r "$1" ]; then
  # Dacă nu are permisiuni, acordă permisiuni temporare de citire
  chmod +r "$1"
  temp_perm=true
else
  temp_perm=false
fi

# Modelul pe care dorim să îl căutăm
pattern="dada"

# Verifică dacă modelul este prezent în fișier
if grep -q "$pattern" "$1"; then
  # Dacă modelul este găsit, returnează 1
  result="DA"
else
  # Dacă modelul nu este găsit, returnează 0
  result="NU"
fi

# Dacă am acordat permisiuni temporare, le revocăm
if [ "$temp_perm" = true ]; then
  chmod -r "$1"
fi
echo $result
