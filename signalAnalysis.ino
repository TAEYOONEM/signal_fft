uint16_t sample[1024]; // ADC 배열 
int stcnt =0; // 0 =STOP, 1=STRAT 
unsigned short value=0; // ADC 단일값
int trigMode = 0; //0 = DEFAULT, 1 = POS_EDGE, 2 = NEG_EDGE
int trigVal = 205; // (1V = 205) , (2V = 410) , (3V = 614 ) , (4V = 820)
unsigned char cmd; // Serial Protocol 입력

void setup() {
  Serial.begin(115200); // Serial 통신 및 ADC 초기화
  init_adc();
}

void loop() 
{  
}

void init_adc()
{
    ADMUX =  (1<<REFS0);   // ADC 초기화 : 기준전압 : AVCC ,  10bit 우측정렬 , ADC0 핀 사용              
    ADCSRA = (1<<ADEN)|(1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC 초기화 : ADC ON , Prescaler Default : 128
}

unsigned short read_adc(){  // ADC 함수 
    unsigned char adc_high,adc_low;
    unsigned short value;
    ADCSRA |= (1<<ADSC);     
    while ((ADCSRA & (1<<ADIF)) == 0);
    adc_low = ADCL;      
    adc_high = ADCH;    
    value = (adc_high << 8) | (adc_low);
    return value;
}

void sample_adc(){   // Trigger 모드에 따른 ADC 
  if(trigMode == 0) { // Default
  for(int i=0;i<1024;i++){
    sample[i] = read_adc();
    }  
   sample_write();
  }  
  else if(trigMode == 1){ // Pose Edge   
     sample[0] = read_adc();
     sample[1] = read_adc();
     if((sample[0]<=trigVal) && (sample[1]>=trigVal)){ 
         for(int i=2;i<1024;i++){
         sample[i] = read_adc();
        }
       sample_write();
      }  
    }  
  else if(trigMode == 2){ // Nega Edge
     sample[0] = read_adc();
     sample[1] = read_adc();
      if((sample[0]>=trigVal) && (sample[1]<=trigVal)){ 
          for(int i=2;i<1024;i++){
            sample[i] = read_adc();
        }
      sample_write();
     }
   }   
}

void sample_write(){ // ADC Serial 통신
   Serial.println("s");
  for(int i=0;i<1024;i++){
  Serial.println(sample[i]);
  }
  Serial.println("e");
}

void sample_all(){  // Start , Stop 제어
  while((!Serial.available())&&(stcnt == 1)){
   sample_adc();
  }
}


void serialEvent(){ // Serial Protocol 에 따른 동작
if(Serial.available()){
    cmd = (char)Serial.read();
     switch (cmd)
  {
   case 'A' :stcnt=1; 
             break; // START   
   case 'B' :stcnt=0; 
            break; // STOP
   case 'C' : 
            break; //  

   // Trigger Mode
   case 'D': trigMode = 0; // DEFAULT
             break;                      
   case 'E': trigMode = 1; // POSE EDGE
             break;                      
   case 'F': trigMode = 2; // NEGA EDGE
             break;      

   //ADC Trigger Valtage
   case 'G': trigVal = 205; // 1V  
             break;             
   case 'H': trigVal = 410; // 2V
             break;                  
   case 'I': trigVal = 614; // 3V
             break;               
   case 'J': trigVal = 820; // 4V
             break;     
   
   // ADC Frequency
   case 'M': ADCSRA &= ~(1<<ADPS2);  // 307.7(kHz) 
             ADCSRA |= (1<<ADPS1);
             ADCSRA &= ~(1<<ADPS0); 
             break;
   case 'O': ADCSRA &= ~(1<<ADPS2); // 153.8(kHz)
             ADCSRA |= (1<<ADPS1);
             ADCSRA |= (1<<ADPS0);             
             break;
   case 'P': ADCSRA |= (1<<ADPS2); // 76.9(kHz)
             ADCSRA &= ~(1<<ADPS1);
             ADCSRA &= ~(1<<ADPS0);             
             break;              
   case 'Q': ADCSRA |= (1<<ADPS2); // 38.5(kHz)
             ADCSRA &= ~(1<<ADPS1);
             ADCSRA |= (1<<ADPS0);            
             break;            
   case 'R': ADCSRA |= (1<<ADPS2); // 19.2(kHz) 
             ADCSRA |= (1<<ADPS1);
             ADCSRA &= ~(1<<ADPS0);                        
             break;             
   case 'S': ADCSRA |= (1<<ADPS2); // 8.982khz
             ADCSRA |= (1<<ADPS1);
             ADCSRA |= (1<<ADPS0);                   
             break;  
    }
    sample_all(); 
 }
}
