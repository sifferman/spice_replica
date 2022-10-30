
#ifndef __CIRCUIT_H
#define __CIRCUIT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <Eigen/Dense>
#include <utility>

class circuit {
public:
    // structs
    struct node {
        int id, name, i;
        std::vector<double> voltages;
        node(int id, int name, int i)
            : id(id), name(name), i(i) { }
        double voltage(const int & t = -1) const;
        void print() const;
    };
    struct linelem {
        enum ElemType_t {
            R = 82,
            C = 67,
            L = 76,
            V = 86,
            I = 73
        };
        circuit & c;
        ElemType_t ElemType;
        node *Node1, *Node2;
        linelem(circuit & c, ElemType_t ElemType, node *Node1, node *Node2)
            : c(c), ElemType(ElemType), Node1(Node1), Node2(Node2) { }
        virtual double voltage(const int & t = -1) const;
        virtual void print() const;
    };
    struct resistor : linelem {
        double resistance;
        resistor(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, double resistance)
            : linelem(c, ElemType, Node1, Node2), resistance(resistance) { }
        double current(const int & t = -1) const;
        void print() const;
    };
    struct storage_device : linelem {
        double current(const int & t = -1) const;
        std::vector<double> currents;
        storage_device(circuit & c, ElemType_t ElemType, node *Node1, node *Node2)
            : linelem(c, ElemType, Node1, Node2) { }
    };
    struct capacitor : storage_device {
        double capacitance, initial_voltage;
        capacitor(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, double capacitance, double initial_voltage)
            : storage_device(c, ElemType, Node1, Node2), capacitance(capacitance), initial_voltage(initial_voltage) { }
        double conductance() const;
        void print() const;
    };
    struct inductor : storage_device {
        double inductance, initial_current;
        inductor(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, double inductance, double initial_current)
            : storage_device(c, ElemType, Node1, Node2), inductance(inductance), initial_current(initial_current) { }
        double conductance() const;
        void print() const;
    };
    struct power_source : linelem {
        enum TYPE_t {
            DC = 0,
            // AC = 1,
            PWL = 2
        };
        TYPE_t SOURCE_TYPE;
        power_source(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE)
            : linelem(c, ElemType, Node1, Node2), SOURCE_TYPE(SOURCE_TYPE) { }
        void print() const;
    };
    struct V_source : power_source {
        virtual double voltage(const int & t = -1) const = 0;
        double current(const int & t = -1) const;
        std::vector<double> currents;
        V_source(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE)
            : power_source(c, ElemType, Node1, Node2, SOURCE_TYPE) { }
        void print() const;
    };
    struct V_dc : V_source {
        double voltage(const int & t = -1) const;
        double voltage_value;
        V_dc(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE, double voltage_value)
            : V_source(c, ElemType, Node1, Node2, SOURCE_TYPE), voltage_value(voltage_value) { }
    };
    struct V_pwl : V_source {
        double voltage(const int & t = -1) const;
        typedef std::vector< std::pair<double, double> > voltages_t;
        voltages_t voltages;
        V_pwl(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE, voltages_t voltages)
            : V_source(c, ElemType, Node1, Node2, SOURCE_TYPE), voltages(voltages) { }
    };
    struct I_source : power_source {
        virtual double current(const int & t = -1) const = 0;
        I_source(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE)
            : power_source(c, ElemType, Node1, Node2, SOURCE_TYPE) { }
        void print() const;
    };
    struct I_dc : power_source {
        double current(const int & t = -1) const;
        double current_value;
        I_dc(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE, double current_value)
            : power_source(c, ElemType, Node1, Node2, SOURCE_TYPE), current_value(current_value) { }
    };
    struct I_pwl : power_source {
        double current(const int & t = -1) const;
        typedef std::vector< std::pair<double, double> > currents_t;
        currents_t currents;
        I_pwl(circuit & c, ElemType_t ElemType, node *Node1, node *Node2, TYPE_t SOURCE_TYPE, double currents)
            : power_source(c, ElemType, Node1, Node2, SOURCE_TYPE), currents(currents) { }
    };
    // struct V_pwl : power_source { };
    // struct I_pwl : power_source { };


    // static variables
    static node * const gnd;
    static const double time_step;



    // functions
    circuit();
    circuit(const std::string & filename);
    ~circuit();

    void step();

    void print() const;
    size_t step_num;

private:
    // helper functions
    void solve(); // use A and z to find currents and voltages

    // time

    // elements and nodes
    std::vector<linelem*> linelems;
    std::unordered_map<int,node*> nodes;

    // matrix model
    Eigen::MatrixXd A;
    size_t n, m;

    // friends
    friend class circuit_loader;
    friend class linelem;
};

#endif