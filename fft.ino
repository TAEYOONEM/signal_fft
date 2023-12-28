#define SIZE 128
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

typedef struct
{
   short real;
   short imag;
} complex;

complex x[SIZE];

short value;
short absFFT[SIZE]; 

void setup() {
  // set prescale to 4
//  cbi(ADCSRA,ADPS2) ;
//  sbi(ADCSRA,ADPS1) ;
//  cbi(ADCSRA,ADPS0) ;
// Serial.begin(115200);

ADMUX = (1<<REFS0);      
ADCSRA = (1<<ADEN) | (1<<ADPS1);  
}
 
void loop() {   
  runLoop();
}

void runLoop(){
  
  int i;
  
  for(i=0;i<SIZE;i++){
    read_adc();
    x[i].real = value;
    x[i].imag = 0;
  }
 FFT(x,SIZE,1);  
 for(int i=0;i<SIZE;i++)
 absFFT[i] = sqrt(x[i].real * x[i].real + x[i].imag * x[i].imag);     
  serialT(); 
  }

void read_adc(){
    unsigned char adc_low, adc_high;
    ADCSRA |= (1<<ADSC);     
    while ((ADCSRA & (1<<ADIF))==0);
    adc_low = ADCL;      
    adc_high = ADCH;    
    value = (adc_high << 8) | adc_low;
}

void FFT(complex g[], unsigned long N, int isign)
{
   
   int i;
   int m;
   double temp_r, temp_i; // 추가
    
   int L, N0;
   
   double wtemp, wr, wpr, wpi, wi, theta;
   double tempr, tempi;


   // 스크램블 수행
   int j = 0;
   for (i = 0; i < N - 1; i++)
   {
      if (j > i)
      {
         temp_r = g[j].real;
         g[j].real = g[i].real;
         g[i].real = temp_r;

         temp_i = g[j].imag;
         g[j].imag = g[i].imag;
         g[i].imag = temp_i;
      }
      m = N >> 1;

      while (j >= m && m >= 2)
      {
         j -= m;
         m >>= 1;
      }
      j += m;
   }
   
   L = 1; // 2원소 DFT로 시작
   while (N >L)
   {
      N0 = L << 1; // N0 = 2*L
      theta = -isign * (6.28318530717959 / N0);
      wtemp = sin(0.5 * theta);
      
      wpr = -2.0 * wtemp * wtemp;
      wpi = sin(theta);

      wr = 1.0;
      wi = 0.0;

      for (m = 0; m < L; m ++)
      {
         for (i = m; i < N; i += N0)
         {
            j = i + L;

            tempr = (wr * g[j].real - wi * g[j].imag);
            tempi = (wr * g[j].imag + wi * g[j].real);

            g[j].real = g[i].real - tempr;
            g[j].imag = g[i].imag - tempi;

            g[i].real += tempr;
            g[i].imag += tempi;
         }
         wr = (wtemp = wr) * wpr - wi * wpi + wr;
         wi = wi * wpr + wtemp * wpi + wi;
      }
      L = N0;
   }
          
   if (isign == -1)
   {
      for (i = 0; i < N; i++)
      {
         g[i].real /= N;
         g[i].imag /= N;
      }
   }
}


void serialT(){
  Serial.begin(9600);
  for(int i=0;i<SIZE;i++){
    Serial.println(absFFT[i]);
    }
while(1){} 
  }
