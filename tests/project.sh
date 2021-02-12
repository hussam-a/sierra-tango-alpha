cd netlist
chmod +x ../../bin/*
for i in *.netlist.v ;
		do
		echo "Processsing file $i ...";
		../../bin/path-lister $i ../lists/$i.list;
				../../bin/sta ../../lib/osu035-updated.lib $i ../caps/$i.cap ../constraints/$i.const ../clk.txt ../paths/$i.paths ../violations/$i.violations;
		../../bin/violations ../../lib/osu035-updated.lib $i ../caps/$i.cap ../constraints/$i.const ../clk.txt ../paths/$i.paths ../violations/$i.violations;
		../../bin/slacks ../../lib/osu035-updated.lib $i ../caps/$i.cap ../constraints/$i.const ../clk.txt ../graphs/$i.dot;
		sed -n -i -e 's/([a-z]+)\[([0-9])\]/$1$2/' ../graphs/$i.dot;
		#dot -Tpng ../graphs/$i.dot -o ../pngs/$i.png;
		done
