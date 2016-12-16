find /Users/aolpin/Downloads/reuters21578\ 2  -type f -iname "*.sgm" -print0 | while IFS= read -r -d $'\0' line; do
     ./preprocessor < "$line"    
done
