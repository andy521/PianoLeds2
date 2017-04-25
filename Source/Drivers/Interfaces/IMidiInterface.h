/**
 * @file
 * @copyright (c) Daniel Schenk, 2017
 * This file is part of mlc2.
 * 
 * @brief Interface for MIDI inputs/outputs.
 */

#ifndef DRIVERS_INTERFACES_IMIDIINTERFACE_H_
#define DRIVERS_INTERFACES_IMIDIINTERFACE_H_

/**
 * Interface for MIDI inputs/outputs.
 */
class IMidiInterface
{
public:
    /** Status type. */
    enum TStatus
    {
        NOTE_OFF = 0x80,
        NOTE_ON = 0x90,
        CONTROL_CHANGE = 0xB0,
    };

    /** Controller number type. */
    enum TControllerNumber
    {
        DAMPER_PEDAL = 0x40,
    };

    static constexpr unsigned int c_numNotes = 256;
    static constexpr unsigned int c_maxNoteNumber = 255;
    static constexpr unsigned int c_numVelocities = 256;
    static constexpr unsigned int c_maxVelocity = 255;

    /**
     * Destructor.
     */
    virtual ~IMidiInterface()
    {
    };

    /**
     * Get number of available ports.
     *
     * @return Number of available ports.
     */
    virtual unsigned int getPortCount() const = 0;

    /**
     * Open a port.
     *
     * @param   number  [in]    Port number to open.
     */
    virtual void openPort(int number) = 0;
};



#endif /* DRIVERS_INTERFACES_IMIDIINTERFACE_H_ */
