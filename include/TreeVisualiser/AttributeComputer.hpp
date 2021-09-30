#pragma once

#include <memory>
#include <vector>

#include <morphotree/tree/mtree.hpp>

struct NormalisedAttributeMeta
{
  std::unique_ptr<std::vector<float>> nattr_;
  float maxValue;
  float minValue;
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

private:
  float normalise(float val, float max, float min) const;  
};