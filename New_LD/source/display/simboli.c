/*
 * simboli.c
 *
 *  Created on: Oct 3, 2014
 *      Author: alvaro.patacchiola
 */

#include "gi_symbols.h"


/*
static struct
   {
   GSYMHEAD sh;        
   SGUCHAR b[13];       
   } m_minuscola_[1] =
   {
   #include "m_minuscola.sym" 
   };
  
static struct
      {
      GSYMHEAD sh;        
      SGUCHAR b[13];       
      } s_maiuscola_[1] =
      {
      #include "s_maiuscola.sym" 
      };
      
         
static struct
            {
            GSYMHEAD sh;        
            SGUCHAR b[13];       
            } u_minuscola_[1] =
            {
            #include "u_minuscola.sym" 
            };
            
            */    

static struct
       {
        GSYMHEAD sh;        /* Symbol header */
        SGUCHAR b[22];       /* Symbol data, "variable length" */
       } check_off_[1] =
       {
        #include "check_off.sym" /* Include symbols */
       };



            
 static struct
            {
             GSYMHEAD sh;        /* Symbol header */
             SGUCHAR b[22];       /* Symbol data, "variable length" */
            } check_on_[1] =
            {
             #include "check_on.sym" /* Include symbols */
            };



static struct
	   {
		GSYMHEAD sh;        /* Symbol header */
		SGUCHAR b[22];       /* Symbol data, "variable length" */
	   } check_yes_[1] =
	   {
		#include "check_yes.sym" /* Include symbols */
	   };




 static struct
			{
			 GSYMHEAD sh;        /* Symbol header */
			 SGUCHAR b[22];       /* Symbol data, "variable length" */
			} check_no_[1] =
			{
			 #include "check_no.sym" /* Include symbols */
			};



            
static struct
            {
               GSYMHEAD sh;        /* Symbol header */
               SGUCHAR b[30];       /* Symbol data, "variable length" */
            } alert_[1] =
            {
               #include "alert.sym" /* Include symbols */
            		
            };
            
  /*
 static struct
             {
              GSYMHEAD sh;
              SGUCHAR b[30];
             } alert2_[1] =
             {
              #include "alert2.sym"
              };
*/

static struct
            {
             GSYMHEAD sh;        /* Symbol header */
             SGUCHAR b[500];       /* Symbol data, "variable length" */
            } alert_big_[1] =
            {
             #include "alert_big.sym" /* Include symbols */
            };


/*
      static struct
            {
             GSYMHEAD sh;
             SGUCHAR b[30];
            } alert_11_off_[1] =
            {
             #include "alert_11_off.sym"
            };
*/
       static struct
            {
             GSYMHEAD sh;        /* Symbol header */
             SGUCHAR b[30];       /* Symbol data, "variable length" */
            } alert_11_on_[1] =
            {
             #include "alert_11_on.sym" /* Include symbols */
            };

            
            static struct
                 {
                  GSYMHEAD sh;        /* Symbol header */
                  SGUCHAR b[30];       /* Symbol data, "variable length" */
                 } pause_[1] =
                 {
                  #include "pause.sym" /* Include symbols */
                 };





 
  static struct
         {
          GSYMHEAD sh;       
          SGUCHAR b[22];       
          } off_[1] =
          {
           #include "off.sym" 
          };     
          
          static struct
                 {
                  GSYMHEAD sh;        /* Symbol header */
                  SGUCHAR b[1764];       /* Symbol data, "variable length" */
                 } off_grande_[1] =
                 {
                  #include "off_grande.sym" /* Include symbols */
                 };
          

         
        
static struct
       {
        GSYMHEAD sh;       
        SGUCHAR b[22];       
       } set_[1] =
       {
        #include "set.sym" 
       };  
       
       
 /*
static struct
       {
        GSYMHEAD sh;       
        SGUCHAR b[22];       
       } home_[1] =
       {
        #include "home.sym" 
       }; 
       */
       
static struct
       {
        GSYMHEAD sh;       
        SGUCHAR b[22];       
       } back_[1] =
       {
        #include "back.sym" 
       };


 static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[22];
        } scroll17_[1] =
        {
         #include "scroll17.sym"
        };


        static struct
               {
                GSYMHEAD sh;
                SGUCHAR b[22];
                } scroll7_[1] =
                {
                 #include "scroll7.sym"
                };



static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[40];
        } scroll6_[1] =
        {
         #include "scroll6.sym"
        };


static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[40];
       } scroll8_[1] =
       {
        #include "scroll8.sym"
       };


static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[40];
       } scroll4_[1] =
       {
        #include "scroll4.sym"
       };


static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[22];
        } scroll10_[1] =
        {
         #include "scroll10.sym"
        };


 static struct
       {
        GSYMHEAD sh;
        SGUCHAR b[44];
       } scrollbar_[1] =
       {
        #include "scrollbar.sym"
       };

                             
       
       
static struct
      {
       GSYMHEAD sh;       
       SGUCHAR b[22];       
      } save_[1] =
      {
       #include "save.sym" 
      };   
      
      
      
static struct
      {
       GSYMHEAD sh;       
       SGUCHAR b[22];       
       } reset_[1] =
       {
        #include "reset.sym" 
       };   
       
       
       
static struct
      {
       GSYMHEAD sh;       
       SGUCHAR b[22];       
       } start_[1] =
       {
        #include "start.sym" 
       };  
       
       
       
 static struct
      {
       GSYMHEAD sh;       
       SGUCHAR b[22];       
      } stop_[1] =
      {
       #include "stop.sym" 
      };                  
       
      
static struct
            {
             GSYMHEAD sh;       
             SGUCHAR b[22];       
            } arrow_down_[1] =
            {
             #include "arrow_down.sym" 
            };  
            
            
  static struct
                        {
                         GSYMHEAD sh;       
                         SGUCHAR b[22];       
                        } arrow_up_[1] =
                        {
                         #include "arrow_up.sym" 
                        };               
      
       
static struct
            {
             GSYMHEAD sh;       
             SGUCHAR b[22];       
            } arrow_right_[1] =
            {
             #include "arrow_right.sym" 
            };                
static struct
			{
			 GSYMHEAD sh;
			 SGUCHAR b[22];
			} next_[1] =
			{
			 #include "next.sym"
			};


	/* static struct
			 {
			  GSYMHEAD sh;
			  SGUCHAR b[22];
			  } hidden_[1] =
			  {
			   #include "hidden.sym"
			  };
*/
/*            
static struct
             {
                GSYMHEAD sh;        
                SGUCHAR b[30];       
             } scroll_[1] =
             {
               #include "arrow_small.sym" 
            };

                 static struct
                        {
                        GSYMHEAD sh;        
                        SGUCHAR b[20];       
                        } s_next_[1] =
                        {
                       #include "next.sym" 
                        };
  
                        
static struct
         {
            GSYMHEAD sh;        
            SGUCHAR b[20];       
         } s_manual_[1] =
         {
          //#include "manual.sym" 
		  #include "manual_small.sym" 
          };
         
                           

static struct
         {
            GSYMHEAD sh;       
            SGUCHAR b[20];       
        } s_off_[1] =
         {
            //#include "off.sym" 
			#include "off_small.sym" 
        };
        
         */
        
        static struct
                 {
                    GSYMHEAD sh;        
                    SGUCHAR b[20];       
                } s_setting_[1] =
                 {
                    #include "settingx.sym"
					//#include "settting_small.sym"
                };

		static struct
			   {
				GSYMHEAD sh;
				SGUCHAR b[22];
			   } info_[1] =
			   {
				#include "info.sym"
			   };

		static struct
			   {
				GSYMHEAD sh;
				SGUCHAR b[500];
			   } alert_pausa_[1] =
			   {
				#include "alert_pausa.sym"
			   };


		static struct
			   {
				GSYMHEAD sh;
				SGUCHAR b[500];
			   } alert_password_[1] =
			   {
				#include "alert_password.sym"
			   };


		static struct
			   {
				GSYMHEAD sh;
				SGUCHAR b[500];
			   } pass_ok_[1] =
			   {
				#include "pass_ok.sym"
			   };

	   static struct
				 {
				  GSYMHEAD sh;
				  SGUCHAR b[22];
				  } abort_sym_[1] =
				  {
				   #include "abort_sym.sym"
				  };

	  static struct
				  {
				   GSYMHEAD sh;        /* Symbol header */
				   SGUCHAR b[7300];       /* Symbol data, "variable length" */
				  } ldosin_logo_[1] =
				  {
				    #include "ldosin_logo.sym" /* Include symbols */
					//#include "lduosin_logo.sym"
				  };

static struct
      {
       GSYMHEAD sh;       
       SGUCHAR b[22];       
       } lan_[1] =
       {
        #include "lan.sym" 
       };

	   
static struct
  {
   GSYMHEAD sh;       
   SGUCHAR b[22];       
   } lan_notconn_[1] =
   {
	#include "lan_notconn.sym" 
   };
   
static struct
{
   GSYMHEAD sh;       
   SGUCHAR b[22];       
   } lan_off_[1] =
   {
	#include "lan_off.sym" 
   };
   
   static struct
	 {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } antenna_[1] =
	  {
	   #include "antenna.sym"
	  };

   static struct
	 {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } sms_[1] =
	  {
	   #include "sms.sym"
	  };

  static struct
	 {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } gprs_[1] =
	  {
	   #include "gprs.sym"
	  };

	   static struct
		 {
		  GSYMHEAD sh;
		  SGUCHAR b[22];
		  } wifi_[1] =
		  {
		   #include "wifi.sym"
		  };

	   static struct
		 {
		  GSYMHEAD sh;
		  SGUCHAR b[22];
		  } wifi1_[1] =
		  {
		   #include "wifi1.sym"
		  };

	  static struct
	 {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } wifi2_[1] =
	  {
	   #include "wifi2.sym"
	  };



	  static struct
	 {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } wifi3_[1] =
	  {
	   #include "wifi3.sym"
	  };

	  static struct
	  {
	  GSYMHEAD sh;
	  SGUCHAR b[22];
	  } wifi4_[1] =
	  {
	   #include "wifi4.sym"
	  };

PGSYMBOL next = & next_;
PGSYMBOL check_off = &check_off_;
PGSYMBOL check_on = & check_on_;
PGSYMBOL check_yes = & check_yes_;
PGSYMBOL check_no = & check_no_;
PGSYMBOL alert = & alert_;
PGSYMBOL alert_big = & alert_big_;
PGSYMBOL alert_11_on = & alert_11_on_;
PGSYMBOL pause = & pause_;
PGSYMBOL off= & off_;
PGSYMBOL off_grande= & off_grande_;
PGSYMBOL set= & set_;
PGSYMBOL back= & back_;
PGSYMBOL scroll6 = & scroll6_;
PGSYMBOL scroll10 = & scroll10_;
PGSYMBOL scroll17 = & scroll17_;
PGSYMBOL scroll7 = & scroll7_;
PGSYMBOL scroll4 = & scroll4_;
PGSYMBOL scroll8 = & scroll8_;
PGSYMBOL scrollbar= & scrollbar_;
PGSYMBOL save= & save_;
PGSYMBOL reset= & reset_;
PGSYMBOL start_sim= & start_;
PGSYMBOL stop_sim= & stop_;
PGSYMBOL arrow_down= & arrow_down_;
PGSYMBOL arrow_up= & arrow_up_;
PGSYMBOL arrow_right= & arrow_right_;
PGSYMBOL info= &info_;
PGSYMBOL alert_pausa= & alert_pausa_;
PGSYMBOL alert_password= & alert_password_;
PGSYMBOL pass_ok= &pass_ok_;
PGSYMBOL abort_sym= &abort_sym_;
PGSYMBOL ldosin_logo= &ldosin_logo_;

PGSYMBOL s_setting= &s_setting_;

//comunicazione
PGSYMBOL lan = &lan_;
PGSYMBOL lan_notconn = &lan_notconn_;
PGSYMBOL lan_off = &lan_off_;
PGSYMBOL antenna = &antenna_;
PGSYMBOL sms = &sms_;
PGSYMBOL gprs = &gprs_;
PGSYMBOL wifi_sym = &wifi_;
PGSYMBOL wifi_sym1 = &wifi1_;
PGSYMBOL wifi_sym2 = &wifi2_;
PGSYMBOL wifi_sym3 = &wifi3_;
PGSYMBOL wifi_sym4 = &wifi4_;

