#include "TreeVisualiser/AttributeComputer.hpp"

#include <morphotree/attributes/attributeComputer.hpp>
#include <morphotree/attributes/bitquads/quadCountComputer.hpp>
#include <morphotree/attributes/volumeComputer.hpp>

#include <algorithm>
#include <math.h>

void NumberOfSkeletonPointCache::openFile(const std::string 
  &cachePath)
{
  file_.open(cachePath, std::ios::out);  

  maxVal_ = std::numeric_limits<int>::min();
  minVal_ = std::numeric_limits<int>::max();
}

void NumberOfSkeletonPointCache::store(unsigned int nodeId,
  int numberOfSkeletonPoints)
{  
  if (maxVal_ < numberOfSkeletonPoints) 
    maxVal_ = numberOfSkeletonPoints;
  
  if (minVal_ > numberOfSkeletonPoints) 
    minVal_ = numberOfSkeletonPoints;

  file_ << nodeId << " " << numberOfSkeletonPoints << "\n";
}

void NumberOfSkeletonPointCache::closeFile()
{
  file_ << maxVal_ << " " << minVal_;
  file_.close();
}

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

  float maxValue = static_cast<float>(quads[0].area());
  float minValue = static_cast<float>(quads[0].area());
  
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

NormalisedAttributeMeta AttributeComputer::computePerimeter(Box domain, 
  const MTree &tree) const
{
  using Quads = morphotree::Quads;
  using AttributeComputer = morphotree::AttributeComputer<Quads, ImageType>;
  using QuadCountComputer = morphotree::CTreeQuadCountsComputer<ImageType>;

  std::vector<ImageType> f = tree.reconstructImage();

  std::vector<Quads> quads =                       
    std::make_unique<QuadCountComputer>(domain, f, "../resource/quads/dt-max-tree-8c.dat")
    ->computeAttribute(tree);

  NormalisedAttributePtr nperimeter = 
    std::make_unique<std::vector<float>>(tree.numberOfNodes());

  float maxValue = static_cast<float>(quads[0].perimeter());
  float minValue = static_cast<float>(quads[0].perimeter());

  for (unsigned int i = 1; i < tree.numberOfNodes(); i++) {
    float perimeter = static_cast<float>(quads[i].perimeter());
    if (maxValue < perimeter) maxValue = perimeter;
    if (minValue > perimeter) minValue = perimeter;
  }

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*nperimeter)[i] = normalise(static_cast<float>(quads[i].perimeter()),
      maxValue, minValue);
  }

  return NormalisedAttributeMeta{ std::move(nperimeter), maxValue, minValue };
}

NormalisedAttributeMeta AttributeComputer::computeVolume(Box domain, const MTree &tree) const
{  
  using AttributeComputer = morphotree::AttributeComputer<float, ImageType>;
  using VolumeComputer = morphotree::MaxTreeVolumeComputer<ImageType>;  

  std::vector<float> volume = 
    std::make_unique<VolumeComputer>()->computeAttribute(tree);

  NormalisedAttributePtr nvolume = std::make_unique<std::vector<float>>(
    tree.numberOfNodes());

  float maxValue = volume[0];
  float minValue = volume[0];

  for (unsigned int i = 1; i < tree.numberOfNodes(); i++) {
    float vol = volume[i];
    if (maxValue < vol) maxValue = vol;
    if (minValue > vol) minValue = vol;
  }

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*nvolume)[i] = normalise(volume[i], maxValue, minValue);
  }

  return NormalisedAttributeMeta{ std::move(nvolume), maxValue, minValue };
}

NormalisedAttributeMeta AttributeComputer::computeCircularity(Box domain, 
  const MTree &tree) const 
{
  using Quads = morphotree::Quads;
  using AttributeComputer = morphotree::AttributeComputer<Quads, ImageType>;
  using QuadCountComputer = morphotree::CTreeQuadCountsComputer<ImageType>;

  std::vector<ImageType> f = tree.reconstructImage();

  std::vector<Quads> quads = std::make_unique<QuadCountComputer>(domain, f,
    "../resource/quads/dt-max-tree-8c.dat")->computeAttribute(tree);

  std::vector<float> circularity(tree.numberOfNodes());
  float maxValue = std::numeric_limits<float>::min();
  float minValue = std::numeric_limits<float>::max();

  NormalisedAttributePtr ncircularity = 
    std::make_unique<std::vector<float>>(tree.numberOfNodes());

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    circularity[i] = (4.f * M_PI * static_cast<float>(quads[i].area())) 
      / static_cast<float>(quads[i] .perimeter());
    if (circularity[i] > maxValue) maxValue = circularity[i];
    if (circularity[i] < minValue) minValue = circularity[i];
  }

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*ncircularity)[i] = normalise(circularity[i], maxValue, minValue);
  }

  return NormalisedAttributeMeta { std::move(ncircularity), maxValue, minValue };
}

NormalisedAttributeMeta AttributeComputer::computeComplexity(Box domain, 
  const MTree &tree) const 
{
  using Quads = morphotree::Quads;
  using AttributeComputer = morphotree::AttributeComputer<Quads, ImageType>;
  using QuadCountComputer = morphotree::CTreeQuadCountsComputer<ImageType>;

  std::vector<ImageType> f = tree.reconstructImage();

  std::vector<Quads> quads = std::make_unique<QuadCountComputer>(domain, f,
    "../resource/quads/dt-max-tree-8c.dat")->computeAttribute(tree);
  
  std::vector<float> complexity(tree.numberOfNodes());
  float maxValue = std::numeric_limits<float>::min();
  float minValue = std::numeric_limits<float>::max();

  NormalisedAttributePtr ncomplexity =
    std::make_unique<std::vector<float>>(tree.numberOfNodes());

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    complexity[i] = static_cast<float>(quads[i].perimeter()) / 
      static_cast<float>(quads[i].area());
    
    if (complexity[i] > maxValue) maxValue = complexity[i];
    if (complexity[i] < minValue) minValue = complexity[i];
  }

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*ncomplexity)[i] = normalise(complexity[i], maxValue, minValue);
  }

  return NormalisedAttributeMeta{ std::move(ncomplexity), maxValue, minValue };
}

NormalisedAttributeMeta AttributeComputer::compueteNumberOfSkeletonPoints(
  const MTree &tree) const
{
  std::ifstream skelCache;
  skelCache.open("./NumberOfSkeletonPoints.txt", std::ios::in);  

  std::vector<int> nskelptList(tree.numberOfNodes());

  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    unsigned int nodeId;
    int nskelpt;
    skelCache >> nodeId >> nskelpt;
    nskelptList[nodeId] = nskelpt;
  }

  int minValTemp, maxValTemp;
  skelCache >> maxValTemp >> minValTemp;

  float maxVal = static_cast<float>(maxValTemp);
  float minVal = static_cast<float>(minValTemp);

  NormalisedAttributePtr nskelptListPtr = 
    std::make_unique<std::vector<float>>(tree.numberOfNodes());
  
  for (unsigned int i = 0; i < tree.numberOfNodes(); i++) {
    (*nskelptListPtr)[i] = normalise(static_cast<float>(nskelptList[i]),
      maxVal, minVal);
  }

  return NormalisedAttributeMeta{std::move(nskelptListPtr), 
    maxVal, minVal };
}

float AttributeComputer::normalise(float val, float max, float min) const
{
  return (val - min) / (max - min);
}