/**
 * @file
 *
 * MIT License
 * 
 * @copyright (c) 2017 Daniel Schenk <danielschenk@users.noreply.github.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @brief Class which represents a concert.
 */

#ifndef PROCESSING_CONCERT_H_
#define PROCESSING_CONCERT_H_

#include <vector>
#include <cstdint>

#include "Interfaces/IJsonConvertible.h"
#include "Interfaces/ProcessingTypes.h"
#include "Common/Scheduler.h"
#include "Drivers/Interfaces/IMidiInterface.h"
#include "Drivers/Interfaces/IMidiInput.h"

class IMidiInput;
class IProcessingBlockFactory;
class IPatch;

/**
 * Class which represents a concert.
 *
 * A concert is a collection of patches, together with some settings which are typically constant
 * throughout a gig, like MIDI channels to listen to and the note-to-light mapping.
 */
class Concert
    : public IJsonConvertible
{
public:
    /**
     * Constructor.
     *
     * @param[in]   rMidiInput              Reference to the MIDI input.
     * @param[in]   rProcessingBlockFactory Reference to the processing block factory.
     */
    Concert(IMidiInput& rMidiInput, IProcessingBlockFactory& rProcessingBlockFactory);

    /**
     * Destructor.
     */
    virtual ~Concert();

    // Prevent implicit constructor, copy constructor and assignment operator.
    Concert() = delete;
    Concert(const Concert&) = delete;
    Concert& operator =(const Concert&) = delete;

    // IJsonConvertible implementation
    virtual Json convertToJson() const;
    virtual void convertFromJson(const Json& rConverted);

    typedef int TPatchPosition;
    static constexpr TPatchPosition c_invalidPatchPosition = -1;

    /**
     * Get the number of patches.
     */
    size_t size() const;

    /**
     * Add a new patch.
     * 
     * @return The patch position, or @ref c_invalidPatchPosition on error.
     */
    TPatchPosition addPatch();

    /**
     * Get the patch at the specified position, for modifications.
     * Concert stays owner of the patch.
     * 
     * @param[in] position  The patch position.
     * 
     * @return Pointer to the patch, or nullptr on error.
     */
    IPatch* getPatch(TPatchPosition position) const;
    
    /**
     * Remove the patch at the specified position.
     * 
     * @param[in] position  The patch position.
     * 
     * @return True on success.
     */
    bool removePatch(TPatchPosition position);

    /**
     * @TODO
     * Move a patch up in the list.
     * 
     * @param[in] position  The patch position.
     * 
     * @return The new patch position.
     */
    // TPatchPosition movePatchUp(TPatchPosition position);

    /**
     * @TODO
     * Move a patch down in the list.
     * 
     * @param[in] position  The patch position.
     * 
     * @return The new patch position.
     */    
    // TPatchPosition movePatchDown(TPatchPosition position);

    bool isListeningToProgramChange() const;
    void setListeningToProgramChange(bool listeningToProgramChange);
    Processing::TNoteToLightMap getNoteToLightMap() const;
    void setNoteToLightMap(Processing::TNoteToLightMap noteToLightMap);
    uint8_t getProgramChangeChannel() const;
    void setProgramChangeChannel(uint8_t programChangeChannel);
    uint16_t getCurrentBank() const;
    void setCurrentBank(uint16_t bank);

    void execute();

protected:
    // IJsonConvertible implementation
    std::string getObjectType() const;

private:
    static constexpr const char* c_isListeningToProgramChangeJsonKey    = "isListeningToProgramChange";
    static constexpr const char* c_noteToLightMapJsonKey                = "noteToLightMap";
    static constexpr const char* c_programChangeChannelJsonKey          = "programChangeChannel";
    static constexpr const char* c_currentBankJsonKey                   = "currentBank";
    static constexpr const char* c_patchesJsonKey                       = "patches";

    typedef std::vector<IPatch*> TPatches;

    /** Callback to handle program changes. */
    void onProgramChange(uint8_t channel, uint8_t program);

    /** Callback to handle control changes. */
    void onControlChange(uint8_t channel, IMidiInterface::TControllerNumber controllerNumber, uint8_t value);

    /** The note-to-light mapping. */
    Processing::TNoteToLightMap m_noteToLightMap;

    /** The collection of patches. */
    TPatches m_patches;

    /** The active patch. */
    TPatches::iterator m_activePatch;

    /** Whether program changes should be able to change the patch. */
    bool m_listeningToProgramChange;

    /** The channel to listen to for program changes. */
    uint8_t m_programChangeChannel;

    /** The last selected bank. */
    uint16_t m_currentBank;
    
    /** Reference to the MIDI input. */
    IMidiInput& m_rMidiInput;

    /** The control change subscription. */
    IMidiInput::TSubscriptionToken m_controlChangeSubscription;
    
    /** The program change subscription. */
    IMidiInput::TSubscriptionToken m_programChangeSubscription;

    /** Reference to the processing block factory. */
    IProcessingBlockFactory& m_rProcessingBlockFactory;

    /** Scheduler to decouple callbacks */
    Scheduler m_scheduler;

    /** Mutex to protect the members. */
    mutable std::mutex m_mutex;
};

#endif /* PROCESSING_CONCERT_H_ */