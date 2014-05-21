/**
 * LowPass helper class for integer values.
 * Daniel Milesson 2014
 */
    
class LowPass
{
  public:
    LowPass(float gain = 0.4);
    
    /** SetValue returns true if the next output differs from the last one */
    bool SetValue(int value);
    int GetValue();
  private:
    float _lastValue;
    float _gain;
    int _outputValue;

};


LowPass::LowPass(float gain) {
  _gain = gain;
  _lastValue = 3E38; // Something really big
}


bool LowPass::SetValue(int value) {
  if(_lastValue >= 3E37) { 
    _lastValue = value; // Set initial value
    _outputValue = value; // Next output will be the first input value
    return true;
  } else {
    // Lowpass filter
     _lastValue = _lastValue + _gain * ( value - _lastValue );
  
    // Store last output
    int _lastOutputValue = _outputValue;
    
    // Calculate the new output value   
    _outputValue = (int) ( _lastValue + 0.5 ); // 0.5 = Integer rounding correction 
  
    // returns true if the next output differs from the last one
    return (_outputValue != _lastOutputValue); 
  }
}


int LowPass::GetValue() {
  return _outputValue; 
}
