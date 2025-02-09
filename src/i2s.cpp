#include <Arduino.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>

// I2S Configuration
#define I2S_WS  15  // Word Select (L/R)
#define I2S_SD  33  // Data In
#define I2S_SCK 25  // Serial Clock
#define I2S_PORT I2S_NUM_0

#define SAMPLE_RATE 16000 // 16 kHz
#define FFT_SAMPLES 1024  // Power of 2

// FFT setup
double realPart[FFT_SAMPLES];
double imagPart[FFT_SAMPLES];

ArduinoFFT<double> FFT = ArduinoFFT<double>(realPart, imagPart, FFT_SAMPLES, SAMPLE_RATE);
// Threshold for detecting significant frequency shift
#define FREQ_THRESHOLD 20.0  // Hz deviation
float lastDominantFreq = 0;



// Function to setup I2S
void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    i2s_start(I2S_PORT);
}

// Function to capture audio and analyze frequency
float detectBeeBuzzFrequency() {
    

    int16_t samples[FFT_SAMPLES];
    size_t bytesRead;
    i2s_read(I2S_PORT, samples, sizeof(samples), &bytesRead, portMAX_DELAY);
    
    if (bytesRead < sizeof(samples)) return -1; // No valid data

    // Convert audio data to real/imag arrays for FFT
    for (int i = 0; i < FFT_SAMPLES; i++) {
        realPart[i] = samples[i] / 32768.0; // Normalize
        imagPart[i] = 0;
    }

   // FFT.windowing(FFT_WIN_HAMMING, FFT_FORWARD);
    FFT.compute(FFT_FORWARD);
    FFT.complexToMagnitude();

    // Find dominant frequency
    int peakIndex = FFT.majorPeak();
    float freq = peakIndex * ((float)SAMPLE_RATE / FFT_SAMPLES);
    
    Serial.printf("Detected Frequency: %.2f Hz\n", freq);
    return freq;
}