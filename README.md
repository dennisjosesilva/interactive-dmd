This software is used for manipulating images interactively.  

# 1. Building


The software needs a C++ compiler, the CUDA, conan, graphviz, and Qt to build. 


# 2. Running

mkdir build

cd build

conan install ..

cmake ..

make

./bin/interactive-dmd 


# 3. Functions


Shape manipulation
---------

**Usage**:
Select node(s) that you want to manipulate in the 'Morphotree' window and then press this icon: <img src="images/Spline_CPs_icon.png" alt="drawing" width="60"/>, then you will enter the shape manipulation window, as shown in the following image.

![ShapeManipulate](images/ShapeManipulate.png)

**Instructions**:

1.  <img src="images/Spline_CPs_icon.png" alt="drawing" width="60"/>
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

12. *Cut CP(s) to another location:* First, select all CPs you want to cut (note that even though only one CP in a branch is selected, the whole branch will be cut), and then press the "X" key. Next, click on the image with the mouse to indicate where you want to put the pasted CPs. Next, click the "V" key, then all the CPs will be pasted to the position you clicked. 


After you have done any of the above operations, click the 'Reconstruct CC' button to view the new/changed reconstructed result of the current shape. You can also click the 'Reconstruct Image' button to view the new/changed reconstructed result of the original image.

# 4. Other remarks



