#pragma once

#include <memory>
#include <vector>
#include <fstream>

#include <morphotree/tree/mtree.hpp>

struct NormalisedAttributeMeta
{
  std::unique_ptr<std::vector<float>> nattr_;
  float maxValue;
  float minValue;
};

class NumberOfSkeletonPointCache
{
public:  
  void openFile(const std::string &cachePath="./NumberOfSkeletonPoints.txt");  
  void store(unsigned int nodeId, int numberOfSkeletonPoints);
  void closeFile();
private:
  std::ofstream file_;
  int maxVal_;
  int minVal_;
};

class AttributeComputer
{  
public:
  using MTree = morphotree::MorphologicalTree<morphotree::uint8>;
  using Box = morphotree::Box;
  using ImageType = morphotree::uint8;
  using NormalisedAttributePtr = std::unique_ptr<std::vector<float>>;

  NormalisedAttributeMeta computeArea(Box domain, const MTree &tree) const;
  NormalisedAttributeMeta computePerimeter(Box domain, const MTree &tree) const;
  NormalisedAttributeMeta computeVolume(Box domain, const MTree &tree) const;
  NormalisedAttributeMeta computeCircularity(Box domain, const MTree &tree) const;
  NormalisedAttributeMeta computeComplexity(Box domain, const MTree &tree) const;
  NormalisedAttributeMeta compueteNumberOfSkeletonPoints(const MTree &tree) const;

private:
  float normalise(float val, float max, float min) const;  
};