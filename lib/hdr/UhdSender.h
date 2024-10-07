#include <uhd/usrp/multi_usrp.hpp>
#include <iostream>
#include <vector>

class UHDSender {
public:
    // Constructor: Initializes the UHD USRP device
    UHDSender(const std::string& device_args) {
        // Create a USRP device using the provided arguments
        usrp = uhd::usrp::multi_usrp::make(device_args);
        
        // Set the sample rate (for example, 1 MHz)
        usrp->set_tx_rate(1e6);

        // Set the center frequency to 2.4 GHz
        usrp->set_tx_freq(2.4e9);

        // Set the gain (for example, 30 dB)
        usrp->set_tx_gain(30.0);
    }

    // Method to send a buffer of samples to the USRP
    void sendSamples(const std::vector<std::complex<float>>& samples) {
        // Create a transmit streamer
        uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(uhd::stream_args_t("fc32"));

        // Send the samples to the USRP
        size_t num_sent = tx_stream->send(&samples.front(), samples.size(), uhd::tx_metadata_t());

        std::cout << "Sent " << num_sent << " samples to the USRP" << std::endl;
    }

private:
    uhd::usrp::multi_usrp::sptr usrp; // UHD USRP device pointer
};
