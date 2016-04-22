FCA_XCODE=~/Documents/Programs/FCA_viz
VIZ=~/Downloads/Research/viz

echo "Edit input file"
open $FCA_XCODE/FCA_viz/input.txt
read -rsp $'Press any key to continue\n' -n1 key

echo "Edit commands file"
open $FCA_XCODE/FCA_viz/com.txt
read -rsp $'Press any key to continue\n' -n1 key

echo "Run program"
open $FCA_XCODE/FCA_viz.xcodeproj
read -rsp $'Press any key to continue\n' -n1 key

echo "Moving coord.txt, edges.txt, obs.txt..."
cp $FCA_XCODE/FCA_viz/coords.txt $VIZ
cp $FCA_XCODE/FCA_viz/edges.txt $VIZ
cp $FCA_XCODE/FCA_viz/obs.txt $VIZ

cd $VIZ
bash viz_local.sh coords.txt edges.txt mst
