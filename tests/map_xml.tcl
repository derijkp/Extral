#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

source tools.tcl

set xml {
<?xml version="1.0"?>
<!DOCTYPE rnaml SYSTEM "http://smi.stanford.edu/people/waugh/RNAML.dtd">
<rnaml>
	<rna name="tRNA">
		<translation-table name="Alli's" number-of-schemes="2" scheme-length="76"  
		comment="Because 'scheme-used-id' is not specified, the default of 'natural' is asumed">
			<numbering-scheme id="1" description="10-offset">
                        10 11 12 13 14 15 16 17 18 19 20
                        21 22 23 24 25 26 27 28 29 30 31 
                        32 33 34 35 36 37 38 39 40 41 42 
                        43 44 45 46 47 48 49 50 51 52 53 
                        54 55 56 57 58 59 60 61 62 63 64 
                        65 66 67 68 69 70 71 72 73 74 75 
                        76 77 78 79 80 81 82 83 84 85 
                </numbering-scheme>
			<numbering-scheme id="2" description="Russ's offset">
                        7 8 9 10 11 12 13 14 15 16 17 18
                        19 20 21 22 23 24 25 26 27 28 29 
                        30 31 32 33 34 35 36 37 38 39 40 
                        41 42 43 44 45 46 47 48 49 50 51 
                        52 53 54 55 56 57 58 59 60 61 62 
                        63 64 65 66 67 68 69 70 71 72 73 
                        74 75 76 77 78 79 80 81 82 
                </numbering-scheme>
		</translation-table>
		<sequence db-source="ndb" db-accession-key="http://ndbserver.rutgers.edu:80/NDB/NDBATLAS/coords/pdb-coord/trna10.pdb">
			<data comment="non A,C,G,U symbols in following sequence mainly provided by
                        tRNAscan-SE home page: http://www.genetics.wustl.edu/eddy/tRNAscan-SE/ ">
                        GCGGAUUURR YUCARUUGGG AGARYGCCAG AYUGAARAUC UGGAGGUYCU GUGUUCRAUY 
                        CACAGAAUUC GCACCA
                 </data>
			<annotations>
				<modified-base base-num="10" type="m2g"/>
				<modified-base base-num="16" type="d"/>
				<modified-base base-num="26" type="m2g"/>
				<modified-base base-num="32" type="cm"/>
				<modified-base base-num="34" type="gm"/>
				<modified-base base-num="37" type="yw"/>
				<modified-base base-num="39" type="psi"/>
				<modified-base base-num="40" type="m5c"/>
				<modified-base base-num="46" type="m7g"/>
				<modified-base base-num="49" type="m5c"/>
				<modified-base base-num="54" type="m5u"/>
				<modified-base base-num="55" type="psi"/>
				<modified-base base-num="58" type="m1a"/>
				<segment id="anticodon loop" start-base-num="34" end-base-num="36"/>
				<segment id="3-prime terminus" start-base-num="73" end-base-num="76"/>
				<segment id="DHU loop" start-base-num="14" end-base-num="21"/>
				<segment id="T-psi-C loop" start-base-num="54" end-base-num="61"/>
			</annotations>
		</sequence>
		<structure comment="source:  http://www.genetics.wustl.edu/eddy/tRNAscan-SE/">
			<base-pair id="BP1" _5p-base-num="1" _3p-base-num="72"/>
			<base-pair id="BP2" _5p-base-num="2" _3p-base-num="71"/>
			<base-pair id="BP3" _5p-base-num="3" _3p-base-num="70"/>
			<base-pair id="BP4" _5p-base-num="4" _3p-base-num="69"/>
			<base-pair id="BP5" _5p-base-num="5" _3p-base-num="68"/>
			<base-pair id="BP6" _5p-base-num="6" _3p-base-num="67"/>
			<base-pair id="BP7" _5p-base-num="7" _3p-base-num="66"/>
			<base-pair id="BP8" _5p-base-num="10" _3p-base-num="25"/>
			<base-triplet id="BT1" base-nums="13 22 46"/>
			<base-triplet id="BT2" base-nums="12 23 9"/>
			<helix id="H1" _5p-base-num="1" _3p-base-num="72" length="7"/>
			<helix id="H2" _5p-base-num="10" _3p-base-num="25" length="4"/>
			<helix id="H3" _5p-base-num="27" _3p-base-num="43" length="5"/>
			<helix id="H4" _5p-base-num="49" _3p-base-num="65" length="5"/>
			<single-strand id="S1" start-base-num="8" length="2"/>
			<single-strand id="S2" start-base-num="14" length="8"/>
			<single-strand id="S3" start-base-num="26" stop-base-num="27"/>
			<single-strand id="S4" start-base-num="32" stop-base-num="39"/>
			<single-strand id="S5" start-base-num="44" length="5"/>
			<single-strand id="S6" start-base-num="54" length="7"/>
			<single-strand id="S7" start-base-num="73" length="4"/>
			<secondary-struct-display comment="These are all approximations from the
                        picture at the above url-source">
				<ss-base-coord base-num="1" position="215.14 516.85"/>
				<ss-base-coord base-num="2" position="215.14 486.12"/>
				<ss-base-coord base-num="3" position="215.14 455.39"/>
				<ss-base-coord base-num="4" position="215.14 421.86"/>
				<ss-base-coord base-num="5" position="215.14 391.13"/>
				<ss-base-coord base-num="6" position="215.14 360.4"/>
				<ss-base-coord base-num="7" position="215.14 329.67"/>
				<ss-base-coord base-num="8" position="195.38 336.64"/>
				<ss-base-coord base-num="9" position="177.8 325.27"/>
				<ss-base-coord base-num="10" position="176.02 304.52"/>
				<ss-base-coord base-num="11" position="142.5 304.52"/>
				<ss-base-coord base-num="12" position="111.77 304.52"/>
				<ss-base-coord base-num="13" position="81.04 304.52"/>
				<ss-base-coord base-num="14" position="59.83 317.92"/>
				<ss-base-coord base-num="15" position="34.76 318.66"/>
				<ss-base-coord base-num="16" position="12.78 306.56"/>
				<ss-base-coord base-num="17" position="0.02 284.97"/>
				<ss-base-coord base-num="18" position="0.0 259.89"/>
				<ss-base-coord base-num="19" position="12.73 238.28"/>
				<ss-base-coord base-num="20" position="34.69 226.15"/>
				<ss-base-coord base-num="21" position="59.76 226.86"/>
				<ss-base-coord base-num="22" position="81.04 240.27"/>
				<ss-base-coord base-num="23" position="111.77 240.27"/>
				<ss-base-coord base-num="24" position="142.5 240.27"/>
				<ss-base-coord base-num="25" position="176.02 240.27"/>
				<ss-base-coord base-num="26" position="188.8 214.93"/>
				<ss-base-coord base-num="27" position="215.14 203.95"/>
				<ss-base-coord base-num="28" position="215.14 173.22"/>
				<ss-base-coord base-num="29" position="215.14 142.49"/>
				<ss-base-coord base-num="30" position="215.14 108.96"/>
				<ss-base-coord base-num="31" position="215.14 78.23"/>
				<ss-base-coord base-num="32" position="201.85 55.19"/>
				<ss-base-coord base-num="33" position="203.84 28.68"/>
				<ss-base-coord base-num="34" position="220.39 7.88"/>
				<ss-base-coord base-num="35" position="245.78 0.0"/>
				<ss-base-coord base-num="36" position="271.21 7.78"/>
				<ss-base-coord base-num="37" position="287.84 28.53"/>
				<ss-base-coord base-num="38" position="289.91 55.04"/>
				<ss-base-coord base-num="39" position="276.6 78.23"/>
				<ss-base-coord base-num="40" position="276.6 108.96"/>
				<ss-base-coord base-num="41" position="276.6 142.49"/>
				<ss-base-coord base-num="42" position="276.6 173.22"/>
				<ss-base-coord base-num="43" position="276.6 203.95"/>
				<ss-base-coord base-num="44" position="294.98 197.66"/>
				<ss-base-coord base-num="45" position="313.55 203.39"/>
				<ss-base-coord base-num="46" position="325.2 218.92"/>
				<ss-base-coord base-num="47" position="325.49 238.35"/>
				<ss-base-coord base-num="48" position="314.31 254.24"/>
				<ss-base-coord base-num="49" position="303.14 268.21"/>
				<ss-base-coord base-num="50" position="333.87 268.21"/>
				<ss-base-coord base-num="51" position="364.6 268.21"/>
				<ss-base-coord base-num="52" position="395.33 268.21"/>
				<ss-base-coord base-num="53" position="426.07 268.21"/>
				<ss-base-coord base-num="54" position="447.76 257.31"/>
				<ss-base-coord base-num="55" position="471.86 260.16"/>
				<ss-base-coord base-num="56" position="490.42 275.78"/>
				<ss-base-coord base-num="57" position="497.33 299.05"/>
				<ss-base-coord base-num="58" position="490.29 322.28"/>
				<ss-base-coord base-num="59" position="471.65 337.81"/>
				<ss-base-coord base-num="60" position="447.54 340.52"/>
				<ss-base-coord base-num="61" position="426.07 329.67"/>
				<ss-base-coord base-num="62" position="395.33 329.67"/>
				<ss-base-coord base-num="63" position="364.6 329.67"/>
				<ss-base-coord base-num="64" position="333.87 329.67"/>
				<ss-base-coord base-num="65" position="303.14 329.67"/>
				<ss-base-coord base-num="66" position="276.6 329.67"/>
				<ss-base-coord base-num="67" position="276.6 360.4"/>
				<ss-base-coord base-num="68" position="276.6 391.13"/>
				<ss-base-coord base-num="69" position="276.6 421.86"/>
				<ss-base-coord base-num="70" position="276.6 455.39"/>
				<ss-base-coord base-num="71" position="276.6 486.12"/>
				<ss-base-coord base-num="72" position="276.6 516.85"/>
				<ss-base-coord base-num="73" position="276.6 543.39"/>
				<ss-base-coord base-num="74" position="276.6 563.88"/>
				<ss-base-coord base-num="75" position="276.6 584.36"/>
				<ss-base-coord base-num="76" position="276.6 604.85"/>
			</secondary-struct-display>
			<base base-num="1">
				<atom name="p" position="24.650 4.594 51.620"/>
			</base>
			<base base-num="2">
				<atom name="p" position="29.640 5.301 54.591"/>
			</base>
			<base base-num="3">
				<atom name="p" position="34.558 4.092 55.472"/>
			</base>
			<base base-num="4">
				<atom name="p" position="38.845 2.801 51.914"/>
			</base>
			<base base-num="5">
				<atom name="p" position="42.128 0.884 47.695"/>
			</base>
			<base base-num="6">
				<atom name="p" position="42.767 0.450 41.759"/>
			</base>
			<base base-num="7">
				<atom name="p" position="41.128 2.411 36.444"/>
			</base>
			<base base-num="8">
				<atom name="p" position="37.555 4.171 31.327"/>
			</base>
			<base base-num="9">
				<atom name="p" position="38.558 5.617 26.671"/>
			</base>
			<base base-num="10">
				<atom name="p" position="35.095 6.450 21.753"/>
			</base>
			<base base-num="11">
				<atom name="p" position="33.284 1.751 24.235"/>
			</base>
			<base base-num="12">
				<atom name="p" position="34.783 -3.267 27.298"/>
			</base>
			<base base-num="13">
				<atom name="p" position="39.176 -4.864 30.657"/>
			</base>
			<base base-num="14">
				<atom name="p" position="45.320 -2.380 32.826"/>
			</base>
			<base base-num="15">
				<atom name="p" position="50.299 -0.095 33.766"/>
			</base>
			<base base-num="16">
				<atom name="p" position="52.658 5.174 35.325"/>
			</base>
			<base base-num="17">
				<atom name="p" position="58.783 7.380 36.302"/>
			</base>
			<base base-num="18">
				<atom name="p" position="56.153 12.293 38.707"/>
			</base>
			<base base-num="19">
				<atom name="p" position="55.690 14.740 33.288"/>
			</base>
			<base base-num="20">
				<atom name="p" position="53.238 17.243 27.600"/>
			</base>
			<base base-num="21">
				<atom name="p" position="50.876 16.180 23.223"/>
			</base>
			<base base-num="22">
				<atom name="p" position="49.849 10.558 20.991"/>
			</base>
			<base base-num="23">
				<atom name="p" position="49.603 4.196 19.130"/>
			</base>
			<base base-num="24">
				<atom name="p" position="49.368 -1.348 17.492"/>
			</base>
			<base base-num="25">
				<atom name="p" position="46.480 -5.780 15.681"/>
			</base>
			<base base-num="26">
				<atom name="p" position="40.942 -6.906 13.106"/>
			</base>
			<base base-num="27">
				<atom name="p" position="36.262 -4.476 10.703"/>
			</base>
			<base base-num="28">
				<atom name="p" position="35.253 -0.591 6.671"/>
			</base>
			<base base-num="29">
				<atom name="p" position="37.039 1.595 1.972"/>
			</base>
			<base base-num="30">
				<atom name="p" position="42.046 1.952 -2.141"/>
			</base>
			<base base-num="31">
				<atom name="p" position="47.250 1.565 -4.785"/>
			</base>
			<base base-num="32">
				<atom name="p" position="50.880 -2.845 -6.159"/>
			</base>
			<base base-num="33">
				<atom name="p" position="50.521 -8.509 -7.546"/>
			</base>
			<base base-num="34">
				<atom name="p" position="47.219 -11.900 -10.223"/>
			</base>
			<base base-num="35">
				<atom name="p" position="42.263 -8.835 -9.558"/>
			</base>
			<base base-num="36">
				<atom name="p" position="39.332 -8.094 -4.798"/>
			</base>
			<base base-num="37">
				<atom name="p" position="38.123 -9.451 1.432"/>
			</base>
			<base base-num="38">
				<atom name="p" position="41.099 -10.250 6.116"/>
			</base>
			<base base-num="39">
				<atom name="p" position="46.305 -10.277 9.860"/>
			</base>
			<base base-num="40">
				<atom name="p" position="51.342 -8.221 10.528"/>
			</base>
			<base base-num="41">
				<atom name="p" position="53.809 -2.389 9.850"/>
			</base>
			<base base-num="42">
				<atom name="p" position="53.365 3.056 9.075"/>
			</base>
			<base base-num="43">
				<atom name="p" position="50.051 7.500 10.169"/>
			</base>
			<base base-num="44">
				<atom name="p" position="45.632 11.343 11.777"/>
			</base>
			<base base-num="45">
				<atom name="p" position="40.314 12.923 14.024"/>
			</base>
			<base base-num="46">
				<atom name="p" position="37.547 12.150 18.559"/>
			</base>
			<base base-num="47">
				<atom name="p" position="35.389 12.461 24.245"/>
			</base>
			<base base-num="48">
				<atom name="p" position="39.512 15.072 29.497"/>
			</base>
			<base base-num="49">
				<atom name="p" position="39.469 9.595 33.844"/>
			</base>
			<base base-num="50">
				<atom name="p" position="36.094 13.806 33.091"/>
			</base>
			<base base-num="51">
				<atom name="p" position="34.176 19.001 34.678"/>
			</base>
			<base base-num="52">
				<atom name="p" position="35.343 23.812 37.825"/>
			</base>
			<base base-num="53">
				<atom name="p" position="39.095 26.932 41.612"/>
			</base>
			<base base-num="54">
				<atom name="p" position="44.776 27.254 44.271"/>
			</base>
			<base base-num="55">
				<atom name="p" position="50.137 27.236 44.414"/>
			</base>
			<base base-num="56">
				<atom name="p" position="53.879 28.091 40.838"/>
			</base>
			<base base-num="57">
				<atom name="p" position="50.439 27.548 36.613"/>
			</base>
			<base base-num="58">
				<atom name="p" position="47.841 22.745 33.940"/>
			</base>
			<base base-num="59">
				<atom name="p" position="46.385 16.863 32.340"/>
			</base>
			<base base-num="60">
				<atom name="p" position="44.981 14.487 37.100"/>
			</base>
			<base base-num="61">
				<atom name="p" position="48.323 10.608 41.680"/>
			</base>
			<base base-num="62">
				<atom name="p" position="46.937 12.287 46.658"/>
			</base>
			<base base-num="63">
				<atom name="p" position="42.865 13.828 50.218"/>
			</base>
			<base base-num="64">
				<atom name="p" position="37.121 14.548 51.290"/>
			</base>
			<base base-num="65">
				<atom name="p" position="31.450 13.612 49.661"/>
			</base>
			<base base-num="66">
				<atom name="p" position="27.820 10.788 45.920"/>
			</base>
			<base base-num="67">
				<atom name="p" position="25.780 6.899 42.000"/>
			</base>
			<base base-num="68">
				<atom name="p" position="25.802 1.843 39.236"/>
			</base>
			<base base-num="69">
				<atom name="p" position="27.633 -3.737 39.128"/>
			</base>
			<base base-num="70">
				<atom name="p" position="29.732 -8.195 41.958"/>
			</base>
			<base base-num="71">
				<atom name="p" position="31.226 -10.948 47.370"/>
			</base>
			<base base-num="72">
				<atom name="p" position="30.858 -11.284 53.336"/>
			</base>
			<base base-num="73">
				<atom name="p" position="27.846 -9.558 58.168"/>
			</base>
			<base base-num="74">
				<atom name="p" position="23.083 -6.943 60.527"/>
			</base>
			<base base-num="75">
				<atom name="p" position="18.015 -6.964 60.608"/>
			</base>
			<base base-num="76">
				<atom name="p" position="12.264 -9.231 58.970"/>
			</base>
			<structural-data comment="This corresponds to experimental data">
				<distance-constraint comment="Some distance measurements derived from model building.
                                        Since no type is specified, it defaults to phosphate." 
						base1-num="64" atom1="p" base2-num="58" atom2="p" mean="21.980" range="2.45"/>
				<distance-constraint base1-num="53" atom1="p" base2-num="76" atom2="p" mean="48.259"/>
				<distance-constraint base1-num="6" atom1="p" base2-num="10" atom2="p" mean="22.251"/>
				<surface-constraint base-num="43" atom="p" surface-value="buried"/>
				<surface-constraint base-num="75" atom="p" surface-value="surface"/>
				<surface-constraint base-num="76" atom="p" surface-value="surface"/>
				<helix-stack helix1-id="H1" helix2-id="H4"/>
				<helix-stack helix1-id="H2" helix2-id="H3"/>
				<base-stack base1-num="34" base2-num="35"/>
				<base-stack base1-num="35" base2-num="36"/>
			</structural-data>
		</structure>
	</rna>
	<rna name="m-RNA">
		<translation-table scheme-length="3"/>
		<sequence>
			<data> AGG	</data>
		</sequence>
	</rna>
	<inter-molecular-interactions>
		<inter-base-pair molecule1="t-RNA" base1-num="34" molecule2="m-RNA" base2-num="3"/>
		<inter-base-pair molecule1="t-RNA" base1-num="35" molecule2="m-RNA" base2-num="2"/>
		<inter-base-pair molecule1="t-RNA" base1-num="36" molecule2="m-RNA" base2-num="1"/>
	</inter-molecular-interactions>
</rnaml>
}

test map_set {set} {
	set map [map_xml $::xml]
	map_fields $map {rnaml rna}
} {translation-table sequence structure}

test map_set {set} {
	set map [map_xml $::xml]
	map_get $map {rnaml rna sequence data _data}
} { GCGGAUUURR YUCARUUGGG AGARYGCCAG AYUGAARAUC UGGAGGUYCU GUGUUCRAUY CACAGAAUUC GCACCA }

test map_set {set} {
	set map [map_xml $::xml]
	map_get $map {rnaml rna sequence annotations modified-base(1)}
} {_args {base-num "16" type "d"}}

test map_set {set} {
	set map [map_xml $::xml]
	map_fields $map {rnaml rna sequence}
} {data annotations _args}

test map_set {set} {
	set map [map_xml $::xml]
	map_get $map {rnaml rna _args}
} {nam tRNA}

test map_set {set} {
	set map [map_xml $::xml]
	map_fields $map rnaml
} {rna rna(1) inter-molecular-interactions}

testsummarize
