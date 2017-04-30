/**
 * @file
 * @copyright (c) Daniel Schenk, 2017
 * This file is part of MLC2.
 */

#include "ProcessingChain.h"
#include "ProcessingBlockFactory.h"

#include "Interfaces/IProcessingBlock.h"

ProcessingChain::ProcessingChain(const ProcessingBlockFactory& rProcessingBlockFactory)
    : m_processingChain()
    , m_rProcessingBlockFactory(rProcessingBlockFactory)
{
}

ProcessingChain::~ProcessingChain()
{
    for(auto pProcessingBlock : m_processingChain)
    {
        delete pProcessingBlock;
    }
}

void ProcessingChain::insertBlock(IProcessingBlock* pBlock, unsigned int index)
{
    if(index > m_processingChain.size())
    {
        index = m_processingChain.size();
    }

    m_processingChain.insert(m_processingChain.begin() + index, pBlock);
}

void ProcessingChain::insertBlock(IProcessingBlock* pBlock)
{
    m_processingChain.insert(m_processingChain.end(), pBlock);
}

json ProcessingChain::convertToJson() const
{
    json converted;
    std::vector<json> convertedChain;
    for(auto pProcessingBlock : m_processingChain)
    {
        convertedChain.push_back(pProcessingBlock->convertToJson());
    }
    converted[c_processingChainJsonKey] = convertedChain;

    return converted;
}

void ProcessingChain::convertFromJson(json converted)
{
    m_processingChain.clear();

    if(converted.count(c_processingChainJsonKey) > 0)
    {
        std::vector<json> convertedChain = converted[c_processingChainJsonKey];
        for(auto convertedBlock : convertedChain)
        {
            m_processingChain.push_back(m_rProcessingBlockFactory.createProcessingBlock(convertedBlock));
        }
    }
}

void ProcessingChain::execute(Processing::TRgbStrip& strip)
{
    for(auto pProcessingBlock : m_processingChain)
    {
        pProcessingBlock->execute(strip);
    }
}
