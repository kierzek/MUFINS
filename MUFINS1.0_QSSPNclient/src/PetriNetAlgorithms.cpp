/*
 *  PetriNetAlgorithms.cpp
 *  pnfba_xcode
 *
 *  Created by Andrzej Kierzek on 06/01/2011.
 *  Copyright 2011 Andrzej Kierzek. All rights reserved.
 *
 */

#include "PetriNet.h"

double Interaction::activity(int i) {
    double result = 0;
    if (activities[i].isMassAction()) {
        result = 1;
        double fact = 1;
        for (int j = 0; j < (int) stoich[i]; j++) {
            result *= (substrates[i]->getState() - j);
            fact *= (j + 1);
        }
        return (result / fact);
    }
    for (int j = 0; j < activities[i].size; j++) {
        result = (substrates[i]->getState() >= activities[i].getThreshold(j)) ? activities[i].getActivity(j) : result;
    }
    return result;
}

double Interaction::propensity(Driver *gsmn) {
    a = c;
    for (int i = 0; i < Ns; i++) a *= activity(i);
    for (int i = 0; i < Nm; i++) metabolites[i]->metaboliteAction(gsmn);
    return a;
}

double Interaction::propensity() {
    if (propensity_expression) {
        for (int i = 0; i < Ns; i++) activity_values[i] = activity(i);
        a = propensityFunction.evaluate();
        return (a);
    }
    a = c;
    for (int i = 0; i < Ns; i++) a *= activity(i);
    return a;

}

void Interaction::stateChange() {
    for (int i = 0; i < Nc; i++) consumed[i]->stateChange += (-1) * a;
    for (int i = 0; i < Np; i++) products[i]->stateChange += a;
}

void Interaction::elementaryChange() {
    for (int i = 0; i < Nc; i++) consumed[i]->stateChange = (-1) * stoich[i];
    for (int i = 0; i < Np; i++) products[i]->stateChange = pstoich[i];
}

void Interaction::execute() {
    stateChange();
    for (int i = 0; i < Nc; i++) consumed[i]->update();
    for (int i = 0; i < Np; i++) products[i]->update();
}

void Interaction::fire() {
    for (int i = 0; i < Nc; i++) consumed[i]->update(-1 * stoich[i]);
    for (int i = 0; i < Np; i++) products[i]->update(pstoich[i]);
}

void Interaction::set() {
    double value;
    value = 1.0;
    if (reset_expression) {
        for (int i = 0; i < Ns; i++) activity_values[i] = activity(i);
        value = resetFunction.evaluate();
    }
    for (int i = 0; i < Np; i++) {
        if (products[i]->resetDone) {
            cout << "Two reset transitions cannot act on place " << products[i]->getName() << " at the same time" << endl;
            exit(0);
        }
        products[i]->resetValue = value * pstoich[i];
        products[i]->resetDone = true;
    }
}

//void Interaction::flux(Driver *gsmn) {
//    double flux;
//    flux = gsmn->getLP()->getColValue(flux_request);
//    for (int i = 0; i < Np; i++) products[i]->setState(flux);
//}

void PetriNet::synchronousUpdate() {
    int i;

    for (i = 0; i < Ne; i++) enzymes[i]->enzymeAction(gsmnDriver);
    for (i = 0; i < Nm; i++) metabolites[i]->fbaRequired(true);
    for (i = 0; i < Ni; i++) {
        interactions[i].propensity(gsmnDriver);
        interactions[i].stateChange();
    }
    for (i = 0; i < Ns; i++) substances[i].update();
}

void PetriNet::prepareForSampling(int ns, int sd) {
    Nsample = ns;
    trajectories = new Trajectory*[Nsample];
    Nia = 0;
    rnd = new RandomNumberGenerator(sd);
}

string PetriNet::asynchronousUpdate() {
    Nia = 0;
    fbaRequired = false;
    for (int k = 0; k < Ne; k++) if (enzymes[k]->enzymeAction(gsmnDriver)) fbaRequired = true;
    if (fbaRequired) for (int k = 0; k < Nm; k++) metabolites[k]->fbaRequired(true);
    for (int j = 0; j < Ni; j++) {
        interactions[j].propensity(gsmnDriver);
        if (interactions[j].a > 0) {
            active[Nia] = &interactions[j];
            Nia++;
        }
    }
    int next = rnd->uniform_index(Nia);
    active[next]->execute();
    return active[next]->getName();
}

/* Test function for nutrient dynamics. */
//void PetriNet::NutrientDynamics(double dt, int max) {
//    int j;
//
//    cout << objective->getName() << "\t";
//    for (j = 0; j < Nn; j++) cout << nutrients[j]->getName() << "\t";
//    cout << objective->getName() << "\t";
//    for (j = 0; j < Nn; j++) cout << nutrients[j]->getName() << "\t";
//    cout << endl;
//
//    buildGsmnDriver();
//
//    for (int i = 0; i < max; i++) {
//        for (j = 0; j < Ne; j++) enzymes[j]->initialEnzymeAction(gsmnDriver);
//        for (j = 0; j < Nn; j++) nutrients[j]->setNutrientBounds(gsmnDriver);
//        objective->objectiveAction(gsmnDriver);
//        for (j = 0; j < Nn; j++) nutrients[j]->nutrientState(gsmnDriver, dt);
//        for (j = 0; j < Nn; j++) nutrients[j]->setNutrientBounds(gsmnDriver);
//
//        cout << objective->getFlux() << "\t";
//        for (j = 0; j < Nn; j++) cout << nutrients[j]->getFlux() << "\t";
//        cout << objective->getState() << "\t";
//        for (j = 0; j < Nn; j++) cout << nutrients[j]->getState() << "\t";
//        cout << endl;
//    }
//}

/************************************************************************************/
/*                        MAIN SIMULATION ENGINE                                    */

/************************************************************************************/

string PetriNet::ssaUpdate() {
    double a0, r2, av;
    int next;

    a0 = 0;
    for (int j = 0; j < Ni; j++) a0 += interactions[j].propensity();
    if (a0 == 0) return (string(NOEVENT));
    deltaTime = 1 / a0;
    Time += deltaTime;
    r2 = rnd->marzam();
    av = 0;
    next = 0;
    while (true) {
        av += interactions[next].a;
        if (av >= a0 * r2) break;
        next++;
    }
    interactions[next].fire();
    return interactions[next].getName();

}

void PetriNet::splitReactions() {
    Ninstant = 0;
    Nfast = 0;
    Nslow = 0;
    int is, fs, ins, rs;
    for (int i = 0; i < Ni; i++)
        if (interactions[i].slow) {
            Nslow++;
        } else if (interactions[i].instant) {
            Ninstant++;
        } else if (interactions[i].reset) {
            Nreset++;
        } else if (interactions[i].fba_flux) {
        } else {
            Nfast++;
        }
    slow = new Interaction*[Nslow];
    fast = new Interaction*[Nfast];
    instant = new Interaction*[Ninstant];
    reset = new Interaction*[Nreset];
    is = 0;
    fs = 0;
    ins = 0;
    rs = 0;
    for (int i = 0; i < Ni; i++) if (interactions[i].slow) {
            slow[is] = &interactions[i];
            is++;
        } else if (interactions[i].instant) {
            instant[ins] = &interactions[i];
            ins++;
        } else if (interactions[i].reset) {
            reset[rs] = &interactions[i];
            rs++;
        } else if (interactions[i].fba_flux) {
            interactions[i].setFluxProducts();
        } else {
            fast[fs] = &interactions[i];
            fs++;
        }

}

void PetriNet::updateFastReactions1() {
    int i;
    string name;
    double buff;

    for (i = 0; i < Nslow; i++) slow[i]->a = 0;
    for (i = 0; i < Nfast; i++) {
        fast[i]->propensity();
        if (fast[i]->instant) {
            fast[i]->a = (fast[i]->a == 0) ? 0 : 1;
        } else {
            fast[i]->a *= deltaTime;
            if (msg_flag) {
                name = fast[i]->getName();
                buff = fast[i]->a;
                cout << "Fast reaction: " + name + " " << buff << endl;
            }
        }
        fast[i]->stateChange();
    }
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4)
            substances[i].update();
}

void PetriNet::updateInstantReactions() {
    int i, next;
    double r, av, a0;
    vector<int> enabled;
    vector<double> propensities;
    vector<int> queue;

    a0 = 0;
    for (i = 0; i < Ninstant; i++) {
        instant[i]->propensity();
        a0 += instant[i]->a;
        if (instant[i]->a > 0) {
            enabled.push_back(i);
            propensities.push_back(instant[i]->a);
        }
    }

    while (!enabled.empty()) {
        r = rnd->marzam();
        av = 0;
        next = 0;
        while (true) {
            av += instant[enabled[next]]->a;
            if (av >= a0 * r) break;
            next++;
        }
        queue.push_back(enabled[next]);
        enabled.erase(enabled.begin() + next);
        a0 -= propensities[next];
        propensities.erase(propensities.begin() + next);
    }

    for (i = 0; i < queue.size(); i++) {
        instant[queue[i]]->propensity();
        if (instant[queue[i]]->a > 0) instant[queue[i]]->fire();
    }

    enabled.clear();
    queue.clear();

}

void PetriNet::updateResetReactions() {
    int i;

    for (i = 0; i < Nreset; i++) {
        reset[i]->propensity();
        if (reset[i]->a > 0) reset[i]->set();
    }
    for (int i = 0; i < Ns; i++) {
        if (substances[i].resetDone)
            substances[i].setState(substances[i].resetValue);
        substances[i].resetDone = false;
    }
}

void PetriNet::updateFastReactions2() {
    /* Naive shortest timestep algorithm for adaptive timestep in Euler ODE integration */
    int i;
    string name;
    double dt, dt1;

    for (i = 0; i < Nslow; i++) slow[i]->a = 0;
    for (i = 0; i < Ninstant; i++) instant[i]->a = 0;
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4)
            substances[i].stateChange = 0;

    // Calculate propensities of fast and instant reactions and pass propenisties as state changes
    for (i = 0; i < Nfast; i++) {
        fast[i]->propensity();
        fast[i]->stateChange();
    }

    // Find minimal timestep such that maximal change is maxChange
    dt = TIME_INF;
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4) {
            if (substances[i].stateChange == 0 || substances[i].getState() == 0) {
                substances[i].stateChange = 0;
                continue;
            }
            dt1 = maxChange * substances[i].getState() / abs(substances[i].stateChange);
            substances[i].stateChange = 0;
            if (dt1 < dt && dt1 > MIN_DT) dt = dt1;
        }

    // set timestep
    deltaTime = (dt > maxTimestep) ? maxTimestep : dt;

    // re-calculate state changes of fast reactions
    for (i = 0; i < Nfast; i++) {
        fast[i]->a *= deltaTime;
        fast[i]->stateChange();
    }

    // update all substances (propensties of slow reactions are still 0)
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4)
            substances[i].update();
}

void PetriNet::updateFastReactions3(double maxdt) {
    /* Naive shortest timestep algorithm for adaptive timestep in Euler ODE integration */
    int i, maxs;
    string name;
    double dt, dt1, sc1, sc;

    for (i = 0; i < Nslow; i++) slow[i]->a = 0;
    for (i = 0; i < Ninstant; i++) instant[i]->a = 0;
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4)
            substances[i].stateChange = 0;

    // Calculate propensities of fast and instant reactions and pass propenisties as state changes
    for (i = 0; i < Nfast; i++) {
        fast[i]->propensity();
        fast[i]->stateChange();
    }

    // Find minimal timestep such that maximal change is maxChange
    dt = TIME_INF;
    maxs = 0;
    sc1 = 0;
    sc = 0;
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4) {
            if (substances[i].stateChange == 0 || substances[i].getState() == 0) {
                substances[i].stateChange = 0;
                continue;
            }
            sc1 = substances[i].stateChange;
            name = substances[i].getName();
            dt1 = maxChange * substances[i].getState() / abs(substances[i].stateChange);
            substances[i].stateChange = 0;
            if (dt1 < dt && dt1 > MIN_DT) {
                dt = dt1;
                sc = sc1;
                maxs = i;
            }
        }

    if (print_max_change)
        cout << "PRINT_MAX_CHANGE Time:" << " " << Time << " Substance:" << substances[maxs].getName() << " Rate:" << sc << endl;

    // set timestep
    deltaTime = (dt > maxdt) ? maxdt : dt;

    // re-calculate state changes of fast reactions
    for (i = 0; i < Nfast; i++) {
        fast[i]->a *= deltaTime;
        fast[i]->stateChange();
    }

    // update all substances (propensties of slow reactions are still 0)
    for (i = 0; i < Ns; i++)
        if (substances[i].getType() < 4)
            substances[i].update();
}

int PetriNet::fireDelayedReaction() {
    int next, eridx;
    double t;

    next = (-1);
    for (int j = 0; j < delays.size(); j++) {
        t = delays[j];
        if (t < Time) {
            next = delayed[j];
            Time = delays[j];
            eridx = j;
        }
    }
    if (next > (-1)) {
        slow[next]->fire();
        delays.erase(delays.begin() + eridx);
        delayed.erase(delayed.begin() + eridx);
        return next;
    }
    return (-1);
}

string PetriNet::hybridUpdate3() {
    double a0, r1, r2, av, tend, tstart, maxdt;
    int next;

    a0 = 0;
    for (int j = 0; j < Nslow; j++) a0 += slow[j]->propensity();
    if (a0 == 0) {
        updateFastReactions3(maxTimestep);
        Time += deltaTime;
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (string(NOEVENT));
    }
    r1 = rnd->marzam();
    deltaTime = (1 / a0) * log(1 / r1);
    if (deltaTime > maxTimestep) {
        tend = Time + maxTimestep;
        while (Time < tend) {
            updateFastReactions3(maxTimestep);
            Time += deltaTime;
        }
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (NOEVENT);
    }
    tstart = Time;
    Time += deltaTime;
    maxdt = deltaTime;
    if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
    r2 = rnd->marzam();
    av = 0;
    next = 0;
    while (true) {
        av += slow[next]->a;
        if (av >= a0 * r2) break;
        next++;
    }
    while (tstart < Time) {
        updateFastReactions3(maxdt);
        tstart += deltaTime;
    }
    if (slow[next]->delay > 0) {
        delayed.push_back(next);
        delays.push_back(Time + slow[next]->delay);
        deltaTime = maxTimestep / 1000;
        return ("delayed:" + slow[next]->getName());
    }
    slow[next]->fire();
    return slow[next]->getName();
}

string PetriNet::hybridUpdate2() {
    double a0, r1, r2, av, tend;
    int next;

    a0 = 0;
    for (int j = 0; j < Nslow; j++) a0 += slow[j]->propensity();
    if (a0 == 0) {
        updateFastReactions2();
        Time += deltaTime;
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (string(NOEVENT));
    }
    r1 = rnd->marzam();
    deltaTime = (1 / a0) * log(1 / r1);
    if (deltaTime > maxTimestep) {
        tend = Time + maxTimestep;
        while (Time < tend) {
            updateFastReactions2();
            Time += deltaTime;
        }
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (NOEVENT);
    }
    Time += deltaTime;
    if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
    r2 = rnd->marzam();
    av = 0;
    next = 0;
    while (true) {
        av += slow[next]->a;
        if (av >= a0 * r2) break;
        next++;
    }
    if (slow[next]->delay > 0) {
        delayed.push_back(next);
        delays.push_back(Time + slow[next]->delay);
        deltaTime = maxTimestep / 1000;
        return ("delayed:" + slow[next]->getName());
    }
    slow[next]->fire();
    return slow[next]->getName();
}

string PetriNet::hybridUpdate1() {
    double a0, r1, r2, av;
    int next;

    a0 = 0;
    for (int j = 0; j < Nslow; j++) a0 += slow[j]->propensity();
    if (a0 == 0) {
        Time += deltaTime;
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (string(NOEVENT));
    }
    r1 = rnd->marzam();
    deltaTime = (1 / a0) * log(1 / r1);
    if (deltaTime > maxTimestep) {
        deltaTime = maxTimestep;
        Time += deltaTime;
        if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
        return (NOEVENT);
    }
    Time += deltaTime;
    if ((next = fireDelayedReaction())>(-1)) return (slow[next]->getName());
    r2 = rnd->marzam();
    av = 0;
    next = 0;
    while (true) {
        av += slow[next]->a;
        if (av >= a0 * r2) break;
        next++;
    }
    if (slow[next]->delay > 0) {
        delayed.push_back(next);
        delays.push_back(Time + slow[next]->delay);
        deltaTime = maxTimestep / 1000;
        return ("delayed:" + slow[next]->getName());
    }
    slow[next]->fire();
    return slow[next]->getName();
}

//void PetriNet::hybridSimulation1() {
//    int i;
//    string rname;
//    ofstream progfile;
//
//    if (msg_flag) cout << "INITIALISE GSMN DRIVER" << endl;
//    buildGsmnDriver();
//    if (msg_flag) cout << "ALLOCATE TRAJECTORIES AND INITIALISE RNG" << endl;
//    prepareForSampling(Nsample, Seed);
//    if (msg_flag) cout << "SPLIT FAST AND SLOW REACTIONS" << endl;
//    splitReactions();
//
//    if (msg_flag) cout << "RECORD INITIAL STATE OF PETRI NET" << endl;
//    initialState = getState();
//
//    for (i = 0; i < Nsample; i++) {
//        Time = 0;
//        deltaTime = 0;
//        timeSinceNutrientUpdate = 0;
//        trajectories[i] = new Trajectory();
//
//        if (msg_flag) cout << endl << "*********** TRAJECTORY " << i << " ***********" << endl;
//        if (msg_flag) cout << "Reload initial state of Petri Net" << endl;
//        setState(initialState);
//
//        while (Time < TimeMax) {
//            if (msg_flag) cout << endl << "ITERATION " << Time << endl;
//            if (substances[targetIdx].getState() >= targetThreshold) break;
//
//            if (msg_flag) cout << "### Update enzyme bounds:" << endl;
//            for (int k = 0; k < Ne; k++) enzymes[k]->setEnzymeBounds(gsmnDriver);
//            if (msg_flag) cout << "### Update nutrient bounds:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->setNutrientBounds(gsmnDriver);
//            if (msg_flag) cout << "### Calculate and constrain objective:" << endl;
//            objective->objectiveAction(gsmnDriver);
//            if (msg_flag) cout << "### Update nutrient state:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->nutrientState(gsmnDriver, deltaTime);
//            if (msg_flag) cout << "### Update nutrient bounds:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->setNutrientBounds(gsmnDriver);
//            if (msg_flag) cout << "### Calculate and constrain objective:" << endl;
//            objective->objectiveAction(gsmnDriver);
//            if (msg_flag) cout << "### Calculate metabolite producibility:" << endl;
//            for (int k = 0; k < Nm; k++) metabolites[k]->producibilityState(gsmnDriver);
//            if (msg_flag) cout << "### Update fast reactions" << endl;
//            updateFastReactions1();
//            if (msg_flag) cout << "### Fire discrete event:" << endl;
//            rname = hybridUpdate1();
//
//            if (msg_flag) cout << Time << "\t" << rname << endl;
//            if (msg_flag) cout << "RECORD THE STATE AT TIME " << Time << endl;
//            runStateMonitors(i, rname, Time);
//        }
//
//        if (i % monitor_fq == 0) {
//            progfile.open(logfile.c_str(), ofstream::app);
//            progfile << "TRAJECTORY " << i << endl;
//            progfile.close();
//        }
//    }
//
//    for (i = 0; i < Nsample; i++) {
//        trajectories[i]->printEventTableWithTime(outfile, i);
//    }
//}
//
//void PetriNet::hybridSimulation2() {
//    int i, Ntarget;
//    string rname;
//    ofstream progfile;
//
//    Ntarget = 0;
//    if (msg_flag) cout << VERSION << endl;
//    if (msg_flag) cout << "Running HYBRID_SIMULATION2" << endl;
//    if (msg_flag) cout << "INITIALISE GSMN DRIVER" << endl;
//    buildGsmnDriver();
//    if (msg_flag) cout << "ALLOCATE TRAJECTORIES AND INITIALISE RNG" << endl;
//    prepareForSampling(Nsample, Seed);
//    if (msg_flag) cout << "SPLIT FAST AND SLOW REACTIONS" << endl;
//    splitReactions();
//
//    if (msg_flag) cout << "RECORD INITIAL STATE OF PETRI NET" << endl;
//    initialState = getState();
//
//    for (i = 0; i < Nsample; i++) {
//        Time = 0;
//        deltaTime = 0;
//        timeSinceNutrientUpdate = 0;
//        trajectories[i] = new Trajectory();
//
//        if (msg_flag) cout << endl << "*********** TRAJECTORY " << i << " ***********" << endl;
//        if (msg_flag) cout << "Reload initial state of Petri Net" << endl;
//        setState(initialState);
//
//        // Calculate initial amounts 
//        for (int k = 0; k < Nn; k++) nutrients[k]->setAmount((double) nutrients[k]->getState() / nutrients[k]->getNutrientUnit());
//        objective->setAmount((double) objective->getState() / BIOMASS_UNIT);
//        if (msg_flag) cout << "SET INITIAL BIOMASS AMOUNT TO: " << objective->getAmount();
//
//        deltaTime = maxTimestep;
//
//        while (Time < TimeMax) {
//            if (msg_flag) cout << endl << "ITERATION " << Time << endl;
//            if (substances[targetIdx].getState() >= targetThreshold) {
//                Ntarget++;
//                break;
//            }
//
//            if (msg_flag) cout << "### Update enzyme bounds:" << endl;
//            for (int k = 0; k < Ne; k++) enzymes[k]->setEnzymeBounds(gsmnDriver);
//            if (msg_flag) cout << "### Update nutrient bounds:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->setNutrientBounds(gsmnDriver, deltaTime, objective->getAmount());
//            if (msg_flag) cout << "### Calculate and constrain objective:" << endl;
//            objective->objectiveAction(gsmnDriver, deltaTime);
//            if (msg_flag) cout << "### Update nutrient state:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->nutrientState(gsmnDriver, deltaTime, objective->getAmount());
//            if (msg_flag) cout << "### Update nutrient bounds:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->setNutrientBounds(gsmnDriver, deltaTime, objective->getAmount());
//            /*if(msg_flag) cout << "### Calculate and constrain objective:" << endl;
//            objective->objectiveAction(gsmnDriver,0.0);*/
//            if (msg_flag) cout << "### Calculate metabolite producibility:" << endl;
//            for (int k = 0; k < Nm; k++) metabolites[k]->producibilityState(gsmnDriver);
//            if (msg_flag) cout << "### Update fast reactions" << endl;
//            updateFastReactions1();
//            if (msg_flag) cout << "### Fire discrete event:" << endl;
//            rname = hybridUpdate1();
//
//            if (msg_flag) cout << Time << "\t" << rname << endl;
//            if (msg_flag) cout << "RECORD THE STATE AT TIME " << Time << endl;
//            if (msg_flag && gsmnDriver->isUpdateRequired()) cout << "UPDATE WAS REQUIRED" << endl;
//            if (msg_flag && !gsmnDriver->isUpdateRequired()) cout << "UPDATE WAS NOT REQUIRED" << endl;
//            runStateMonitors(i, rname, Time);
//            gsmnDriver->setUpdateRequired(false);
//        }
//
//        if (i % monitor_fq == 0) {
//            progfile.open(logfile.c_str(), ofstream::app);
//            progfile << "TRAJECTORY " << i << endl;
//            progfile.close();
//        }
//    }
//
//    progfile.open(logfile.c_str(), ofstream::app);
//    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
//    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
//    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
//    progfile.close();
//
//    for (i = 0; i < Nsample; i++) {
//        trajectories[i]->printEventAmountTableWithTime(outfile, i);
//    }
//}

/*****************************************************************************************/
/*                           WORKHORSE SIMULATION ROUTINE                                */

/*****************************************************************************************/

void PetriNet::hybridSimulation3Standalone() {
    int i, Ntarget;
    string rname;
    ofstream progfile;

    buildGsmnDriver();
    prepareForSampling(Nsample, Seed);
    splitReactions();

    initialState = getState();

    Ntarget = 0;
    for (i = 0; i < Nsample; i++) {
        Time = 0;
        deltaTime = 0;
        timeSinceNutrientUpdate = 0;
        trajectories[i] = new Trajectory();
        setState(initialState);
        deltaTime = maxTimestep;
        rname = "none";
        runStateMonitors(i, rname, Time);

        iteration_counter = 0;
        while (Time < TimeMax) {
            if (target_flag) if (substances[targetIdx].getState() >= targetThreshold) {
                    Ntarget++;
                    break;
                }

            for (int k = 0; k < Ne; k++) enzymes[k]->setEnzymeBounds(gsmnDriver);
            for (int k = 0; k < Nm; k++) metabolites[k]->producibilityStateStandalone(gsmnDriver);
            updateResetReactions();
            updateInstantReactions();
            rname = hybridUpdate3();
            gsmnDriver->setUpdateRequired(false);

            iteration_counter++;
            if (iteration_counter % monitor_fq == 0) runStateMonitors(i, rname, Time);
            if (progress_flag && iteration_counter % progress_fq == 0) cout << "Trajectory: " << i << " Time: " << Time << endl;
        }

        progfile.open(logfile.c_str(), ofstream::app);
        progfile << "TRAJECTORY " << i << endl;
        progfile.close();

    }

    progfile.open(logfile.c_str(), ofstream::app);
    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
    progfile << endl;
    progfile << VERSION;
    progfile.close();

    for (i = 0; i < Nsample; i++) {
        trajectories[i]->printEventAmountTableWithTime(outfile, i);
    }
}

double PetriNet::runOptimization(std::string clientString) {
    if (gsmnDriver == NULL) {
       buildGsmnDriver();
    } else {
        gsmnDriver->reset_changes();
	//delete gsmnDriver;
	//buildGsmnDriver();
    }
    gsmnDriver->restoreChangesMapAndUpdateLP(clientString);
    double res = gsmnDriver->calcObjValue();
    return res;
}

void PetriNet::hybridSimulation3Client(std::string serverAddress,  std::string serverPort) {
    int i, Ntarget;
    string rname;
    ofstream progfile;

    buildGsmnDriver();
    gsmnDriver->set_server_address(serverAddress, serverPort);
    prepareForSampling(Nsample, Seed);
    splitReactions();

    initialState = getState();

    Ntarget = 0;
    for (i = 0; i < Nsample; i++) {
        Time = 0;
        deltaTime = 0;
        timeSinceNutrientUpdate = 0;
        trajectories[i] = new Trajectory();
        setState(initialState);
        deltaTime = maxTimestep;
        rname = "none";
        runStateMonitors(i, rname, Time);

        iteration_counter = 0;
        while (Time < TimeMax) {
            if (target_flag) if (substances[targetIdx].getState() >= targetThreshold) {
                    Ntarget++;
                    break;
                }

            for (int k = 0; k < Ne; k++) enzymes[k]->setEnzymeBounds(gsmnDriver);
            for (int k = 0; k < Nm; k++) metabolites[k]->producibilityStateBuffClient(gsmnDriver);
            updateResetReactions();
            updateInstantReactions();
            rname = hybridUpdate3();
//            gsmnDriver->setUpdateRequired(false);

            iteration_counter++;
            if (iteration_counter % monitor_fq == 0) runStateMonitors(i, rname, Time);
            if (progress_flag && iteration_counter % progress_fq == 0) cout << "Trajectory: " << i << " Time: " << Time << endl;
        }

        progfile.open(logfile.c_str(), ofstream::app);
        progfile << "TRAJECTORY " << i << endl;
        progfile.close();

    }

    progfile.open(logfile.c_str(), ofstream::app);
    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
    progfile << endl;
    progfile << VERSION;
    progfile.close();

    for (i = 0; i < Nsample; i++) {
        trajectories[i]->printEventAmountTableWithTime(outfile, i);
    }
}



void PetriNet::pnSimulation() {
    int i, Ntarget;
    string rname;
    ofstream progfile;

    prepareForSampling(Nsample, Seed);
    splitReactions();

    initialState = getState();

    Ntarget = 0;
    for (i = 0; i < Nsample; i++) {
        Time = 0;
        deltaTime = 0;
        timeSinceNutrientUpdate = 0;
        trajectories[i] = new Trajectory();
        setState(initialState);
        deltaTime = maxTimestep;
        runStateMonitors(i, rname, Time);

        iteration_counter = 0;
        while (Time < TimeMax) {
            if (target_flag) if (substances[targetIdx].getState() >= targetThreshold) {
                    Ntarget++;
                    break;
                }

            updateResetReactions();
            updateInstantReactions();
            rname = hybridUpdate3();

            iteration_counter++;
            if (iteration_counter % monitor_fq == 0) runStateMonitors(i, rname, Time);
            if (progress_flag && iteration_counter % progress_fq == 0) cout << "Trajectory: " << i << " Time: " << Time << endl;
        }

        progfile.open(logfile.c_str(), ofstream::app);
        progfile << "TRAJECTORY " << i << endl;
        progfile.close();

    }

    progfile.open(logfile.c_str(), ofstream::app);
    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
    progfile << endl;
    progfile << VERSION;
    progfile.close();

    for (i = 0; i < Nsample; i++) {
        trajectories[i]->printEventAmountTableWithTime(outfile, i);
    }
}

//void PetriNet::hybridSimulation4() {
//    int i, Ntarget;
//    string rname;
//    ofstream progfile;
//
//    Ntarget = 0;
//    if (msg_flag) cout << VERSION << endl;
//    if (msg_flag) cout << "Running HYBRID_SIMULATION4" << endl;
//    if (msg_flag) cout << "INITIALISE GSMN DRIVER" << endl;
//    buildGsmnDriver();
//    if (msg_flag) cout << "ALLOCATE TRAJECTORIES AND INITIALISE RNG" << endl;
//    prepareForSampling(Nsample, Seed);
//    if (msg_flag) cout << "SPLIT FAST AND SLOW REACTIONS" << endl;
//    splitReactions();
//
//    if (msg_flag) cout << "RECORD INITIAL STATE OF PETRI NET" << endl;
//    initialState = getState();
//
//    for (i = 0; i < Nsample; i++) {
//        Time = 0;
//        deltaTime = 0;
//        timeSinceNutrientUpdate = 0;
//        trajectories[i] = new Trajectory();
//
//        if (msg_flag) cout << endl << "*********** TRAJECTORY " << i << " ***********" << endl;
//        if (msg_flag) cout << "Reload initial state of Petri Net" << endl;
//        setState(initialState);
//
//        // Calculate initial amounts
//        for (int k = 0; k < Nn; k++) nutrients[k]->setAmount((double) nutrients[k]->getState() / nutrients[k]->getNutrientUnit());
//        objective->setAmount((double) objective->getState() / BIOMASS_UNIT);
//        if (msg_flag) cout << "SET INITIAL BIOMASS AMOUNT TO: " << objective->getAmount();
//
//        deltaTime = maxTimestep;
//
//        while (Time < TimeMax) {
//            if (msg_flag) cout << endl << "ITERATION " << Time << endl;
//            if (substances[targetIdx].getState() >= targetThreshold) {
//                Ntarget++;
//                break;
//            }
//
//            if (msg_flag) cout << "### Update enzyme bounds:" << endl;
//            for (int k = 0; k < Ne; k++) enzymes[k]->setEnzymeBounds(gsmnDriver);
//
//            if (msg_flag) cout << "### Update nutrient bounds:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->setNutrientBounds(gsmnDriver, deltaTime, objective->getAmount());
//
//            if (msg_flag) cout << "### Calculate metabolite producibility:" << endl;
//            gsmnDriver->getLP()->setColBnds(objective->getGsmnReaction(), LB_INF, UB_INF);
//            if (msg_flag) cout << "### Removing constraints from:" << objective->getGsmnReaction() << endl;
//            for (int k = 0; k < Nm; k++) metabolites[k]->producibilityState(gsmnDriver);
//
//            if (msg_flag) cout << "### Calculate and constrain objective:" << endl;
//            objective->objectiveAction(gsmnDriver, deltaTime);
//
//            if (msg_flag) cout << "### Update nutrient state:" << endl;
//            for (int k = 0; k < Nn; k++) nutrients[k]->nutrientState(gsmnDriver, deltaTime, objective->getAmount());
//
//            if (msg_flag) cout << "### New algorithm functions for hybrid update" << endl;
//            updateResetReactions();
//            updateInstantReactions();
//            rname = hybridUpdate2();
//
//            if (msg_flag) cout << Time << "\t" << rname << endl;
//            if (msg_flag) cout << "RECORD THE STATE AT TIME " << Time << endl;
////            if (msg_flag && gsmnDriver->isUpdateRequired()) cout << "UPDATE WAS REQUIRED" << endl;
////            if (msg_flag && !gsmnDriver->isUpdateRequired()) cout << "UPDATE WAS NOT REQUIRED" << endl;
//            runStateMonitors(i, rname, Time);
////            gsmnDriver->setUpdateRequired(false);
//        }
//
//        if (i % monitor_fq == 0) {
//            progfile.open(logfile.c_str(), ofstream::app);
//            progfile << "TRAJECTORY " << i << endl;
//            progfile.close();
//        }
//    }
//
//    progfile.open(logfile.c_str(), ofstream::app);
//    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
//    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
//    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
//    progfile << endl;
//    progfile << VERSION;
//    progfile.close();
//
//    for (i = 0; i < Nsample; i++) {
//        trajectories[i]->printEventAmountTableWithTime(outfile, i);
//    }
//}

void PetriNet::tokenGame1() {
    int i, Ntarget;
    string rname;
    ofstream progfile;

    Ntarget = 0;
    if (msg_flag) cout << "ALLOCATE TRAJECTORIES AND INITIALISE RNG" << endl;
    prepareForSampling(Nsample, Seed);
    if (msg_flag) cout << "SPLIT FAST AND SLOW REACTIONS" << endl;
    splitReactions();

    if (msg_flag) cout << "RECORD INITIAL STATE OF PETRI NET" << endl;
    initialState = getState();

    for (i = 0; i < Nsample; i++) {
        Time = 0;
        deltaTime = 0;
        trajectories[i] = new Trajectory();

        if (msg_flag) cout << endl << "*********** TRAJECTORY " << i << " ***********" << endl;
        if (msg_flag) cout << "Reload initial state of Petri Net" << endl;
        setState(initialState);

        deltaTime = maxTimestep;

        while (Time < TimeMax) {
            if (msg_flag) cout << endl << "ITERATION " << Time << endl;
            if (substances[targetIdx].getState() >= targetThreshold) {
                Ntarget++;
                break;
            }

            if (msg_flag) cout << "### Update fast reactions" << endl;
            updateFastReactions1();
            if (msg_flag) cout << "### Fire discrete event:" << endl;
            rname = hybridUpdate1();

            if (msg_flag) cout << Time << "\t" << rname << endl;
            if (msg_flag) cout << "RECORD THE STATE AT TIME " << Time << endl;
            runStateMonitors(i, rname, Time);
        }

        if (i % monitor_fq == 0) {
            progfile.open(logfile.c_str(), ofstream::app);
            progfile << "TRAJECTORY " << i << endl;
            progfile.close();
        }
    }

    progfile.open(logfile.c_str(), ofstream::app);
    progfile << "NUMBER OF TRAJECTORIES REACHING TARGET: " << Ntarget << endl;
    progfile << "TOTAL NUMBER OF TRAJECTORIES: " << Nsample << endl;
    progfile << "RATIO OF TRAJECTORIES REACHING TARGET: " << (float) Ntarget / (float) Nsample << endl;
    progfile << endl;
    progfile << VERSION;
    progfile.close();

    for (i = 0; i < Nsample; i++) {
        trajectories[i]->printEventAmountTableWithTime(outfile, i);
    }

}

/************************************************************************************/

void RandomNumberGenerator::rmarzam(int newseed) {

    /* initialization variables */
    int ij, kl, i, j, k, l, m, ii, jj;
    float s, t;

    /* random number variables */
    int seed; /* default seed if none specified */

    seed = newseed % 900000000;
    ij = seed / 30082;
    kl = seed - 30082 * ij;
    i = ((ij / 177) % 177) + 2;
    j = (ij % 177) + 2;
    k = ((kl / 169) % 178) + 1;
    l = (kl % 169);

    for (ii = 0; ii < 97; ii++) {

        s = 0.0;
        t = 0.5;

        for (jj = 0; jj < 24; jj++) {

            m = (((i * j) % 179) * k) % 179;
            i = j;
            j = k;
            k = m;
            l = ((53 * l) + 1) % 169;
            if (((l * m) % 64) >= 32)
                s += t;
            t *= .5;
        }

        u[ii] = s;
    }

    c = 362436. / 16777216.;
    cd = 7654321. / 16777216.;
    cm = 16777213. / 16777216.;
    i96 = 96;
    j96 = 32;
}

double RandomNumberGenerator::marzam() {
    /* random number generation */
    uni = u[i96] - u[j96];
    if (uni < 0.) uni += 1.0;
    u[i96] = uni;
    i96--;
    if (i96 < 0) i96 = 96;
    j96--;
    if (j96 < 0) j96 = 96;
    c -= cd;
    if (c < 0.) c += cm;
    uni -= c;
    if (uni < 0.) uni += 1.0;

    return ( (double) uni);
}


