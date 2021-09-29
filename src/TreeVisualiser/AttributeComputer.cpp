#include "TreeVisualiser/AttributeComputer.hpp"

#include <morphotree/attributes/attributeComputer.hpp>
#include <morphotree/attributes/bitquads/quadCountComputer.hpp>

#include <algorithm>


NormalisedAttributeMeta AttributeComputer::computeArea(Box domain, const MTree &tree) const
{
  using Quads = morphotree::Quads;
  using AttributeComputer = morphotree::AttributeComputer<Quads, ImageType>;
  using QuadCountComputer = morphotree::CTreeQuadCountsComputer<ImageType>;
  
  std::vector<ImageType> f = tree.reconstructImage();

  std::vector<Quads> quads = 
    std::make_unique<QuadCountComputer>(domain, f, "../resource/quads/dt-max-tree-8c.dat")
    ->computeAttribute(tree);

  NormalisedAttributePtr narea = 
    std::make_unique<std::vector<float>>(tree.numberOfNodes());

  float maxValue = quads[0].area();
  float minValue = quads[0].area();
  
  for(unsigned int i = 1; i < tree.numberOfNodes(); i++) {
    float area = static_cast<float>(quads[i].area());
    if (maxValue < area) maxValue = area;
    if (minValue > area) minValue = area;
  }

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*narea)[i] = normalise(static_cast<float>(quads[i].area()),
      maxValue, minValue);
  }

  return NormalisedAttributeMeta{std::move(narea), maxValue, minValue};
}

float AttributeComputer::normalise(float val, float max, float min) const
{
  return (val - min) / (max - min);
}