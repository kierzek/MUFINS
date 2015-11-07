/* 
 * File:   DriverGrb.h
 * Author: John
 *
 * Created on 24 June 2014, 14:21
 */

#ifndef DRIVERGRB_H
#define	DRIVERGRB_H

#include "Driver.h"
#include "Gurobi.h" //w/grb/


class DriverGrb: public Driver{
private:
    //Gurobi model-----------------------------
    GRBEnv* env;//w/grb/

protected:
    //void init_msg_grb();//w/grb/    
    void objval();
    void objstat();
    void fba();
    void fva();
    void fva_gim3e(double value);//for gim3e
    void ko();//w/
    void gfaa();//w/gfa/
    void gfva();//w/gfa/
	void gfva2();//w/gfa/
	//void gfva3();//w/gfa/
    void gfva2x();//w/gfa/
    void sgni();//w/gni/
    void wgni(GRB_Mod* mod);//w/gni/
    void wgni2(GRB_Mod* mod);//w/gni/
 //   void wgniGrb1(GRB_Mod* mod);//w/gni/
    void ess();//w/gni/
    void dpaplot();//w/dpa/

    //---------------GRB--------------------------------------------------------
    void blp_fba(GRB_Mod *bmod, BLPSOL &bsol, stomap &medium);//w/grb/
    void blp_fba(GRB_Mod *bmod, BLPSOL &bsol);//w/grb/
    OBJSTAT fba_med(GRB_Mod *mod, intvec rmed);//w/grb/

public:
    GRB_Mod* grbmod;//w/grb/

    DriverGrb();
    virtual ~DriverGrb();

    void run();
    void set_solver(char *arg);
    //static string get_msg_grb(int nmsg);//w/grb/

    friend bool read_record_Grb(DriverGrb*, string);//w/grb/
    friend void read_constraint_grb(DriverGrb*, string, char);//w/grb/
    friend void read_objective_Grb(DriverGrb*, string);//w/grb/

    
};


#endif	/* DRIVERGRB_H */

