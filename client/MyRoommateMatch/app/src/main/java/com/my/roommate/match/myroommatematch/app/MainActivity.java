package com.my.roommate.match.myroommatematch.app;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.content.Intent;
import android.content.Context;
//import android.R;

import javax.net.ssl.SSLContext;


import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.io.*;
import java.lang.*;


class Student{
    public String email;
    public String first;
    public String last;
    public String gender;
    public String year;
    public Boolean smoke;
    public Boolean greek;
    public Boolean shared;
    public Boolean snore;
    public Boolean party;
    public Boolean bed;
    public String religion;
    public String[] sports = new String[10];
    public int grades;

    public Student(){
        //DO STUFF
        this.email = null;
    }
}


public class MainActivity extends ActionBarActivity {
    byte[] mybytes;
    public int submitflg;

    private RadioGroup genderGrp, yearGrp, smokeGrp, sharedGrp, snoreGrp, partyGrp, bedGrp, greekGrp;
    private RadioButton genderBt, yearBt, smokeBt, sharedBt, snoreBt, partyBt, bedBt, greekBt;
    private CheckBox chkBasketball, chkBaseball, chkSoccer,
            chkFootball, chkUltimate, chkSwimming, chkHockey,
            chkSoftBall, chkTennis, chkOther;
    SeekBar grades;
    EditText email, first, last, religion, otherSport;
    Button submit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        System.out.println("HERE!!!!");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        addListenerOnButton();
        //addListenerOnSubmit();
    }

    public void addListenerOnButton() {
        final Context context = this;

        System.out.println("in listener");
        final Student myStudent = new Student();
        genderGrp = (RadioGroup) findViewById(R.id.genderGrp);
        yearGrp = (RadioGroup) findViewById(R.id.yearGrp);
        smokeGrp = (RadioGroup) findViewById(R.id.smokeGrp);
        sharedGrp = (RadioGroup) findViewById(R.id.shareGrp);
        snoreGrp = (RadioGroup) findViewById(R.id.snoreGrp);
        partyGrp = (RadioGroup) findViewById(R.id.partyGrp);
        bedGrp = (RadioGroup) findViewById(R.id.bedGrp);
        greekGrp = (RadioGroup) findViewById(R.id.greekGrp);

        grades = (SeekBar) findViewById(R.id.grades);

        email = (EditText) findViewById(R.id.email);
        last = (EditText) findViewById(R.id.last);
        first = (EditText) findViewById(R.id.first);
        religion = (EditText) findViewById(R.id.religion);
        otherSport = (EditText) findViewById(R.id.other_sport);

        //sports
        chkBasketball = (CheckBox) findViewById(R.id.basketball);
        chkBaseball = (CheckBox) findViewById(R.id.baseball);
        chkFootball = (CheckBox) findViewById(R.id.football);
        chkSoccer = (CheckBox) findViewById(R.id.soccer);
        chkUltimate = (CheckBox) findViewById(R.id.ultimate);
        chkSwimming = (CheckBox) findViewById(R.id.swimming);
        chkSoftBall = (CheckBox) findViewById(R.id.softball);
        chkHockey = (CheckBox) findViewById(R.id.hockey);
        chkTennis = (CheckBox) findViewById(R.id.tennis);
        chkOther = (CheckBox) findViewById(R.id.other);
        //end sports

        submit = (Button) findViewById(R.id.submit);


        submit.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                System.out.println("in onClick");
                //get selected radio button from radio Group
                int selectGender = genderGrp.getCheckedRadioButtonId();
                int selectYear = yearGrp.getCheckedRadioButtonId();
                int selectSmoke = smokeGrp.getCheckedRadioButtonId();
                int selectShare = sharedGrp.getCheckedRadioButtonId();
                int selectSnore = snoreGrp.getCheckedRadioButtonId();
                int selectParty = partyGrp.getCheckedRadioButtonId();
                int selectBed = bedGrp.getCheckedRadioButtonId();
                int selectGreek = greekGrp.getCheckedRadioButtonId();

                //find the radio button by the returned id
                genderBt = (RadioButton) findViewById(selectGender);
                yearBt = (RadioButton) findViewById(selectYear);
                smokeBt = (RadioButton) findViewById(selectSmoke);
                sharedBt = (RadioButton) findViewById(selectShare);
                snoreBt = (RadioButton) findViewById(selectSnore);
                partyBt = (RadioButton) findViewById(selectParty);
                bedBt = (RadioButton) findViewById(selectBed);
                greekBt = (RadioButton) findViewById(selectGreek);

                //sports
                int i=0;
                if(chkBasketball.isChecked()) {
                    myStudent.sports[i] = "basketball";
                    i++;
                }
                if(chkFootball.isChecked()){
                    myStudent.sports[i] = "football";
                    i++;
                }
                if(chkBaseball.isChecked()){
                    myStudent.sports[i] = "baseball";
                    i++;
                }
                if(chkSoftBall.isChecked()){
                    myStudent.sports[i] = "softball";
                    i++;
                }
                if(chkTennis.isChecked()){
                    myStudent.sports[i] = "tennis";
                    i++;
                }
                if(chkHockey.isChecked()){
                    myStudent.sports[i] = "hockey";
                    i++;
                }
                if(chkSwimming.isChecked()){
                    myStudent.sports[i] = "swimming";
                    i++;
                }
                if(chkUltimate.isChecked()){
                    myStudent.sports[i] = "ultimate";
                    i++;
                }
                if(chkSoccer.isChecked()){
                    myStudent.sports[i] = "soccer";
                    i++;
                }
                if(chkOther.isChecked()){
                    myStudent.sports[i] = otherSport.getText().toString();
                    i++;
                }

                int j=0;
                while(j<10){
                    if(myStudent.sports[j]!=null) {
                        System.out.println("Sports:" + myStudent.sports[j]);
                    }
                    j++;
                }

                Boolean smokeflg = null;
                if (smokeBt.getText().equals("No")) {
                    smokeflg = false;
                } else if (smokeBt.getText().equals("Yes")) {
                    smokeflg = true;
                } else {
                    System.out.println("Smoke is not set");
                }

                Boolean shareflg = null;
                if(sharedBt.getText().equals("No")){
                    shareflg = false;
                }else if(sharedBt.getText().equals("Yes")){
                    shareflg = true;
                }else{
                    System.out.println("Share is not set");
                }

                Boolean snoreflg = null;
                if(snoreBt.getText().equals("No")){
                    snoreflg = false;
                }else if(snoreBt.getText().equals("Yes")){
                    snoreflg = true;
                }else{
                    System.out.println("Snore is not set");
                }

                Boolean partyflg = null;
                if(partyBt.getText().equals("No")){
                    partyflg = false;
                }else if(partyBt.getText().equals("Yes")){
                    partyflg = true;
                }else{
                    System.out.println("Party is not set");
                }

                Boolean bedflg = null; //if student is an early bird?
                if(bedBt.getText().equals("Early to bed, Early to rise")){
                    bedflg = true;
                }else if(bedBt.getText().equals("Late to bed, Late to rise")){
                    bedflg = false;
                }else{
                    System.out.println("Bed is not set");
                }

                Boolean greekflg = null;
                if(greekBt.getText().equals("Yes")){
                    greekflg = true;
                }else if(greekBt.getText().equals("No")){
                    greekflg = false;
                }else{
                    System.out.println("Greek is not set");
                }

                myStudent.email = email.getText().toString();
                myStudent.first = first.getText().toString();
                myStudent.last = last.getText().toString();
                myStudent.gender = genderBt.getText().toString();
                myStudent.year = yearBt.getText().toString();
                myStudent.smoke = smokeflg;
                myStudent.shared = shareflg;
                myStudent.snore = snoreflg;
                myStudent.party = partyflg;
                myStudent.bed = bedflg;
                myStudent.religion = religion.getText().toString();
                myStudent.grades = grades.getProgress();
                myStudent.greek = greekflg;


                System.out.println(myStudent.email);
                System.out.println(myStudent.first);
                System.out.println(myStudent.last);
                System.out.println(myStudent.gender);
                System.out.println(myStudent.year);
                System.out.println(myStudent.smoke);
                System.out.println(myStudent.shared);
                System.out.println(myStudent.snore);
                System.out.println(myStudent.party);
                System.out.println(myStudent.bed);
                System.out.println(myStudent.religion);
                System.out.println(myStudent.grades);

                String myFinalString = "{\n"+
                "\"email\": " + myStudent.email + ";\n"+
                "\"fname\": " + myStudent.first + ";\n"+
                "\"lname\": " + myStudent.last + ";\n"+
                "\"gender\": " + myStudent.gender + ";\n"+
                "\"year\": " + myStudent.year + ";\n"+
                "\"smoking\": " + myStudent.smoke + ";\n"+
                "\"pledge\": " + myStudent.greek + ";\n"+
                "\"drink\": " + myStudent.party + ";\n"+
                "\"religion\": " + myStudent.religion + ";\n"+
                "\"shared_before\": " + myStudent.shared + ";\n"+
                "\"early_bird\": " + myStudent.bed + ";\n"+
                "\"snore\": " + myStudent.snore + ";\n"+
                "\"importance_of_grades\": " + myStudent.grades + ";\n"+
                "\"sports\"" + "(" + i + "):" + " [";

                int k;
                for(k=0; k<i; k++){
                //while(k<i){
                    if(k!=(i-1)) {
                        myFinalString = myFinalString + myStudent.sports[k];
                        myFinalString = myFinalString + ",";
                    } else {
                        myFinalString = myFinalString + myStudent.sports[k];
                    }
                }

                myFinalString = myFinalString + "];\n";
                myFinalString = myFinalString + "}";
                System.out.println(myFinalString);
                mybytes = myFinalString.getBytes();

                Thread myclient = new Thread(){

                    @Override
                    public void run(){
                        System.out.println("in ClientThread");
                        try {
                            InetAddress serverAddr = InetAddress.getByName(SEVER_IP);
                            sock = new Socket(serverAddr, SERVERPORT);
                            OutputStream os = sock.getOutputStream();
                            os.write(mybytes, 0, mybytes.length);
                            sock.close();
                        }catch(Exception e){
                            e.printStackTrace();
                        }
                    }
                };
                myclient.start();

                //Switch to second Profile
                Intent intent = new Intent (context, Second_Profile.class);
                startActivity(intent);

            }
        });

        chkOther.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(chkOther.isChecked()) {
                    otherSport.setEnabled(true);
                    otherSport.setClickable(true);
                    otherSport.setFocusable(true);
                    otherSport.setFocusableInTouchMode(true);
                    otherSport.setCursorVisible(true);
                } else {
                    otherSport.setEnabled(false);
                    otherSport.setClickable(false);
                    otherSport.setFocusable(false);
                    otherSport.setFocusableInTouchMode(false);
                    otherSport.setCursorVisible(false);
                    otherSport.setText("");
                }
            }
        });

    }

    private Socket sock;
    private static final int SERVERPORT = 3377;
    private static final String SEVER_IP = "128.10.2.13";


    class ClientThread implements Runnable {
        Boolean connected = true;
        @Override
        public void run() {
            System.out.println("in ClientThread");
            try {
                InetAddress serverAddr = InetAddress.getByName(SEVER_IP);
                sock = new Socket(serverAddr, SERVERPORT);
                OutputStream os = sock.getOutputStream();
                os.write(mybytes, 0, mybytes.length);
                while (connected) {
                    try {
                        System.out.println("in try");
                       // PrintWriter out = new PrintWriter (new BufferedWriter(new OutputStreamWriter(sock.getOutputStream())), true);
                       // out.println("HERE!!!");
                    } catch (Exception e1) {
                        e1.printStackTrace();
                    }
                }
                sock.close();
            }catch(Exception e){
                e.printStackTrace();
            }
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

}