This software is used for manipulating images interactively.  

# 1. Building

The software needs a C++ compiler, the CUDA, [conan](https://conan.io/),  and Qt to build. 
It also depends on some custom Conan packages that can be installed manually or using a bash script on 
Linux.

## 1.1. Manual dependency installation

To install a Conan package, we need to run a Conan command at the root of the repositories on the links down below. 

- [morphotree library](https://github.com/dennisjosesilva/morphotree) 

```bash
$ conan create . morphotree/0.0.1@dennisjosesilva/dev
```

- [IcicleMorphotreeWidget](https://github.com/dennisjosesilva/IcicleMorphotreeWidget)

```bash
$ conan create . IcicleMorphotreeWidget/0.0.1@dennisjosesilva/dev
```

- [ImageViewerWidget](https://github.com/dennisjosesilva/ImageViewerWidget)

```bash
$ conan create . ImageViewerWidget/0.0.1@dennisjosesilva/dev
```

- [Spline](https://github.com/WangJieying/Spline)

```bash
$ conan create . Spline/0.0.1@jieyingwang/dmd
```

- [Skel](https://github.com/WangJieying/Skel)

```bash
$ conan create . Skel/0.0.1@jieyingwang/dmd
```

## 1.2 Scripted dependency installation

Alternatively to the manual Conan packages installation. We also provided a bash script to install all custom Conan packages dependencies. It runs Conan recipes that automatically download the dependencies from GitHub and install it at the local Conan. 

The scripts are on the directory "**conan-dependencies-recipes**".  To run this script in Linux, one can run the following command at the **conan-dependencies-recipes** directory:

```bash
$ bash create_dependencies_packages.bash
```

We also provide a script that removes all custom Conan package dependencies. To run this script, one can run the following command at the **conan-dependencies-recipes** directory:

```bash
$ bash remove_dependencies_packages.bash
```

Then, enter with yes for the packages to be removed and hit **Enter** (or **Return**).

There is also a bash script to update an individual Conan package. This script will remove the chosen conan package and install the new package from the github repository. To do it, we can run the following command at the **conan-dependencies-recipes** directory:

```bash
$ bash update_package.bash
```

Then, it will ask to chose a package as shown in the image below:

![](images/update-package.png)

we chose the package by typing the index (1-5) correspoding to the package name on the prompt. After the updating, the command to install the conan packages has to be run again and the project rebuilt (see **section 2**). 

# 2. Running

mkdir build

cd build

conan install ..

cmake ..

make

./bin/interactive-dmd 

# 3. Functions

### Morphotree widget and node selection

The morphotree widget is an alternative representation of the image. In this widget, we render a morphhological tree (either a max-tree or a min-tree). In this tree, each node represents a connected component of an upper-threshold or lower-thresdhold. The child nodes are connected components that are subsets of their parent node. An simple example is shown down below:

![](images/simple-tree-example.png)

In this figure, we can see tree nodes representing the connected components in white that span from gray to white at the graphical representation of tree. These nodes are children of a bigger node that spans from black to gray. This node contains the previously small tree nodes. Finally the root node in black that contains the bigger node.

### Usage:

### Show morphological tree

To show the morphological tree of the current image one can click on <img src="images/morphotree_icon.png" title="" alt="" width="50">  button. The rendered tree depends on the selected  tree type. We can chose between min-tree (to edit dark objects on bright background) or max-tree (to edit bright objects on dark background) by change the value of the tree type combo box as shown in the figure below:

![](images/min-tree.png)

#### Node selection and unselection

The main way to interact of the morphotree widget is to select nodes. We can select nodes using two ways:

1) *Direct node selection*: we can select a node by click on it on the morphotree widget. When selected, the node got a red border.

2) *Pixel node selection*: we can activated node selection by pixel clicking on <img title="" src="images/node_selection_pixel_icon.png" alt="" data-align="inline" width="50">  . When it is activated, we can click at a pixel of the input image and select smallest node that contains that pixel.

In both ways, we can select more than a node by clicking on the node or pixel with the *SHIFT* key down. Also, we can select all descendant nodes of the nodes that are already selected by clicking on  <img src="images/select_descendants_icon.png" title="" alt="" width="51">. 

When we click in a node that is already selected, it gets unselected. If the _SHIFT_ key is down, the selection of the other nodes are kept, otherwise all nodes gets unselected.

#### Node visualisation and reconstruction

We can visualise the connected component represented by the selected nodes by two ways:

1. *Visualise direct on the image*: We can visualise the nodes direct in the image viewer, by activating this features using the button <img src="images/highlight_node_icon.png" title="" alt="" width="68"> . When it is activated, the connected component represented by the node is painted in red at the image viewer.

2. _Node reconstruction:_ We can visualise the selected nodes in an dock window as a binary image. To do it, we can press the button <img title="" src="images/binrec_plus_icon.png" alt="" width="54"> to reconstruct the selected nodes.

#### Manipulate tree rendering

We can also manipulate some aspects of the tree rendering for a better visualition. Down below we list some of these commands:

1. *Zoom in*: We can apply a zoom in at the tree rendering by pressing *+* (plus) or clicking on <img src="images/image_zoom_in_icon.png" title="" alt="" width="53">.

2. *Zoom out*: We can apply a zoom out at the tree rendering by pressing *-* (minus) or clicking on <img src="images/image_zoom_in_icon.png" title="" alt="" width="57">.

3. *Fit to window*: We can fit the tree rendering to the dock window by pressing *0* (zero) or clicking on <img src="images/fit_to_widget_icon.png" title="" alt="" width="50">.

4. *Rotate tree*: We can rotate the tree to display it vertically or horizontally by clicking on <img src="images/rotate_icon.png" title="" alt="" width="50">.

5. *Pan*: We can navigate the rendering using pan by holding *Alt* key down.

#### Attributes visualisation

We can visualise some attributes using a colourmap rendering of the nodes. To do it, we can click on *Attribute Visualisation* at the menu bar and chose an attribute to visualise. To remove the attribute visualisation, we have to click in the *Attribute visualisation* menu and unchecked the attribute.

#### Removing and include nodes for SDMD reconstruction

Using the morphotree widget, we can also remove nodes from the SDMD reconstruction and add then back. The commands to do that are described down below:

1. *Removing nodes*: We can remove the selected nodes from the DMD reconstruction by clicking on <img src="images/icicle_node_removal_icon.png" title="" alt="" width="50">. When we do it, the removed nodes are rendering using semi-transparent colours.

2. *Including nodes*: We can include back the selected removed nodes by click on <img src="images/icicle_node_inclusion_icon.png" title="" alt="" width="50">. When we do it, the remove nodes are rendering using the full (no transparency) colour again.

3. We can also apply the DMD method for connected component removal by clicking on the button *Run* below the menu bar. The selection of the nodes corresponds to the DMD connected components removal strategy defined by the parameters on the spin boxes. After clicking on run the removed nodes are rendering in semi-transparent colours at the morphotree widget.

4. We reconstruct the image without the removed nodes using the SDMD pipeline by clicking on <img src="images/Skel_icon.png" title="" alt="" width="50">. After clicking on it, the image on the left panel will show the reconstruction result.

#### Tree synchronisation

The tool can change the skeleton of the nodes to produce other connected  components as described at the section **Shape manipulation**. When this happen, the reconstruction image gets unsynchronised with the tree. In this case, the node selection funcionality can get unexpected results. In order to make the manipulated image corresponding to the max-tree of the widget the user should synchronise the tree. 

When tree gets unsynchronised (the user apply a skeleton manipulation) the border of the image gets yellow  and a message is shown in the status bar as shown in the image below:

![](images/unsync_example.png)

To synchronise the image to the tree, the user can click on <img src="images/sync_morphotree_icon.png" title="" alt="" width="50"> at the toolbar. When the image and tree is synchronised, it cannot go back to the previous status and the whole tree and node skeletons are recomputed.

SDMD reconstruction
---------

This program also provides a way to run SDMD [(Spline-Based Dense Medial Descriptors)](https://www.mdpi.com/2313-433X/7/8/153) reconstruction interactively. User-controllable parameters in the control bar are marked in the red box below.

![](images/SDMD-example.png)

SDMD has four important parameters that affect the trade-off between image size and quality. The meaning of
these parameters is straightforward: L determines how many layers (image intensities for a grayscale image) are used for the reconstruction; ε controls the maximum size of small-scale details that are removed; σ controls how much to smooth isophote or isochrome contours in an image; and γ tells how accurately B-splines fit the MAT, i.e., how precisely we want to encode the position and shape of objects in an image.

Apart from that, users can also check the 'Interp' checkbox to get an interpolated result. After setting all parameters, one can run 'Run' button to get the SDMD reconstructed result. Users can also click the 'OrigImg' checkbox to check the original image.

There are two benefits to running the SDMD reconstruction. First, it provides an indirect way to select nodes on the morphotree. As shown in the above figure, when L is set to 10, then only 10 gray levels are selected on the morpotree in the right window. Secondly, for some complex grayscale images (such as images with many small details), it will take a lot of time to directly encode all its morpotree nodes. In this case, once can choose to run SDMD first, and then encode, i.e., click on <img src="images/iciclemorphotree_icon.png" alt="drawing" width="50"/>. This way, although not all the information of the original image is encoded, most of the information is retained, and importantly, the encoding time is greatly reduced.

Another thing is that these parameters in the control bar also control the skeletons/splines that represent the node/component, i.e., during the encoding process, each tree node/component generates a skeleton/spline based on these parameters.

Shape manipulation
---------

**Usage**:
Select node(s) that you want to manipulate in the 'Morphotree' window and then press this icon: <img src="images/Spline_CPs_icon.png" alt="drawing" width="60"/>, then you will enter the shape manipulation window, as shown in the following image.

![ShapeManipulate](images/ShapeManipulate.png)

**Instructions**:

1. <img src="images/Spline_CPs_icon.png" alt="drawing" width="60"/>

After pressing this button, the control points (CPs) of the shape(s) will be displayed. Each shape has one or several branches. Each branch has one or more CPs. The color of each branch indicates its degree, and their corresponding relationship is displayed at the bottom of the window.

2. <img src="images/Remove_SplineCPs_icon.png" alt="drawing" width="60"/>

This button will undisplay all CPs.

3. <img src="images/AddCP_icon.png" alt="drawing" width="60"/>

This button is used for adding one CP in a branch. So first you need to select a branch (by clicking any CP on it), then click the button, next click where you want to add the CP. Note that if you want to add a CP in the middle of a branch, please click between the rectangle formed by the front and back CPs. If not, you will add a new branch with two CPs.

4. <img src="images/DeleteCP_icon.png" alt="drawing" width="60"/>

This button is used for deleting one CP in a branch. So first you need to click the CP that you want to delete and then click this button.

5. <img src="images/DeleteABranch_icon.png" alt="drawing" width="60"/>

This button is used for deleting a whole branch. So first you need to select a branch (by clicking any CP on it) that you want to delete and then click this button.

6. <img src="images/DeleteMultiCP_icon.png" alt="drawing" width="60"/>

This button is used for deleting multiple CPs. You can select multiple CPs at once by the rubber band drag mode of the mouse, and then click this button to delete all the selected CPs.

7. <img src="images/rotate_CPs.png" alt="drawing" width="60"/>

This button is used for rotating all the selected CPs. First, you need to click this button and then click on the image where you want to put focus. Next, select all the CPs that you want to process. Then hold down the "R" key and scroll the mouse wheel to the angle you want.

8. <img src="images/zoom_in_out.png" alt="drawing" width="60"/>

This button is used for scaling all the selected CPs. First, you need to click this button and then click on the image where you want to put focus. Next, select all the CPs that you want to process. Then hold down the "Z" key and scroll the mouse wheel to scale up/down of the CPs.

In addition to the above operations, you can also **move** CP(s) directly on the shape, change the **radius** or **degree** of a CP, and **copy**/**cut** CP(s), as follows.

9. *Change the radius:* First, select a CP, and then hold down the shift key, and next scroll the mouse wheel to increase/decrease the radius. Its radius is shown on the right corner of the window.

10. *Chang the degree:* First, select a CP, and then hold down the "D" key, and next scroll the mouse wheel to increase/decrease the degree. Its degree is shown on the right corner of the window. Note that after changing the degree of this CP, the degree of other CPs on the same branch will also be changed.
    For detailed explanation about *degree* of a spline, please check this [link](https://www.geometrictools.com/Documentation/BSplineReduction.pdf).

11. *Copy CP(s) to another location:* First, select all CPs you want to copy (note that even though only one CP in a branch is selected, the whole branch will be copied), and then press the "C" key. Next, click on the image with the mouse to indicate where you want to put the pasted CPs. Next, click the "V" key, then all the CPs will be copied to the position you clicked. 

12. *Cut CP(s) to another location:* First, select all CPs you want to cut (note that even though only one CP in a branch is selected, the whole branch will be cut), and then press the "X" key. Next, click on the image with the mouse to indicate where you want to put the pasted CPs. Next, click the "V" key, then all the CPs will be pasted to the position you clicked. Note that unlike copy, the cut CPs can only be pasted *once*.

After you have done any of the above operations, click the 'Reconstruct CC' button to view the new/changed reconstructed result of the current shape. You can also click the 'Reconstruct Image' button to view the new/changed reconstructed result of the original image.

A shape manipulation example can be found [here](images/demo.mp4).

![ShapeManipulate](images/horseManipulate.png)

# 4. Other remarks
