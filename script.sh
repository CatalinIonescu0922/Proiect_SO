word_to_search=$1;
file_path=$2;
if [grep "$word_to_search" "$file_path" -eq 0] then 
    exit(0)
fi 