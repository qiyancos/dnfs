#include <iostream>
#include "executer/job_execute.h"
using namespace std;

class JobA : public JobBase
{
    bool execute() override
    {
        cout << "execute in JobA" << endl;
        return true;
    }
};

int main()
{
    JobExecute job_execute(5);
    JobA job = {};
    job_execute.execute(&job);
    return 0;
}
