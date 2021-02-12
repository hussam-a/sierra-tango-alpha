cd netlist
chmod +x ../../bin/gen-net-cap
for i in *.netlist.v ; do ../../bin/gen-net-cap "$i" "../caps/$i.cap" "../constraints/$i.const" ;echo "Processsing file $i ..."; done
