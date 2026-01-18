{
echo -n 'dist_pkgdata_DATA ='
ls *jpg *dat *ble *ttf *txt *bmp 2> /dev/null | while read line; do 

echo ' \'
echo -n "$line"
done

echo
} > Makefile.am
