package_dirs=(
  "./IcicleMorphotreeWidget"
  "./morphotree"
  "./ImageViewerWidget"
  "./Skel"
  "./Spline");

package_name=(
  "IcicleMorphotreeWidget"
  "morphotree"
  "ImageViewerWidget"
  "Skel"
  "Spline");

package_conan=(
  "IcicleMorphotreeWidget/0.0.1@dennisjosesilva/dev"
  "morphotree/0.0.1@dennisjosesilva/dev"
  "ImageViewerWidget/0.0.1@dennisjosesilva/dev"
  "Skel/0.0.1@jieyingwang/dmd"
  "Spline/0.0.1@jieyingwang/dmd"
);

echo "Packages to updated:";
echo "(1) IcicleMorphotreeWidget";
echo "(2) morphotree";
echo "(3) ImageViewerWidget";
echo "(4) Skel";
echo "(5) Spline";
echo "Enter the index of the package to be updated: ";
read opt

if [[ $opt -ge 1 ]] && [[ $opt -le 5 ]] 
then
  let package_index=opt-1
  conan remove ${package_conan[$package_index]} --force;  
  echo ""
  echo "Package ${package_name[$package_index]} has been removed.";  
  echo "======================================================================================";
  conan create ${package_dirs[$package_index]} ${package_conan[$package_index]};

  echo "";
  echo "======================================================================================";
  echo "Package ${package_name[$package_index]} has been created again with the updated code.";
else
  echo "Index ${package_index} is Invalid.";
fi