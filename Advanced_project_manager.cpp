#include <string>
#include <iostream>
using namespace std;

// Q1: Exception Classes [15 pts]
class ProjectException : public exception {
protected:
    string message;

public:
    ProjectException(const string& msg) : 
        message(msg) {}
    const char* what() const noexcept override {
        return message.c_str(); // convert string to char*
    }

};

class InvalidProjectException : public ProjectException {
public:
    InvalidProjectException(int projectId) :
        ProjectException("Invalid project ID: " + to_string(projectId)) {}
};

class CapacityExceededException : public ProjectException { 
public:
    CapacityExceededException(const string& description) :
        ProjectException("Capacity exceeded: " + description) {}
};

class TaskNotFoundException : public ProjectException { 
public:
    TaskNotFoundException(int taskId, int projectId) :
        ProjectException("Task ID " + to_string(taskId) + " not found in project " + to_string(projectId)) {}
};

class MemberNotFoundException : public ProjectException {
public:
    MemberNotFoundException(int memberId) :
        ProjectException("Member ID " + to_string(memberId) + " not found") {}
};

class AssignmentException : public ProjectException {
public:    
    AssignmentException(const string& reason) :
        ProjectException("Assignment failed: " + reason) {}
};


// Q2: Base and Specialized Task Classes [25 pts]
// 1) upgrades to task class
class Task {
private:
    int taskId;
    string description;
    string status;
    int priority; // 1 to 5
    bool isAssigned;

public:
    Task(int taskId, const string& description, int priority, const string& status) 
        : taskId(taskId), description(description), status(status), isAssigned(false) {
            setPriority(priority); 
        } 

    virtual ~Task() = default;   // no dynamic memory alocation, so default is enough

    Task(const Task& other) = default;

    void updateStatus(const string& newStatus){
        status = newStatus;
    }

    // changing this function for A3
    virtual void display() const{
        cout << "Task ID: " << taskId 
        << ", Description: " << description 
        << ", Priority:" << priority 
        << ", Status: " << status << '\n';
    }

    int getTaskId() const{
        return taskId;
    }

    int getPriority() const{
        return priority;
    }

    // status can't be modified when called nor in the function.
    const string& getStatus() const{
        return status;
    }

    // make sure new priority is between 1 and 5
    void setPriority(int p){
        if (p < 1 || p > 5) {
            throw ProjectException("Priority must be between 1 and 5");
        }

        priority = p;
    }

    bool getAssignedStatus() const{
        return isAssigned;
    }

    void markAssigned(){
        isAssigned = true;
    }

    void markUnassigned(){
        isAssigned = false;
    }
};

// 2) sub classes of task
class DevelopmentTask : public Task {
private:
    string programmingLanguage;

public:
    DevelopmentTask(int taskId, const string& description, int priority, const string& status, const string& programmingLanguage) 
    : Task(taskId, description, priority, status), programmingLanguage(programmingLanguage) {} // uses Task constructor to start, then sets up programmingLanguage

    void display() const override {
        Task::display(); // uses parent display to show some of the info of the parent, then adds programming language info
        cout << "Programming Language: " << programmingLanguage << '\n';
    }

    const string& getProgrammingLanguage() const{
        return programmingLanguage;
    }
 };

class TestingTask : public Task {
private:
    string testType;
public:
    TestingTask(int taskId, const string& description, int priority, const string& status, const string& testType) 
    : Task(taskId, description, priority, status), testType(testType) {} // uses Task constructor to start, then sets up testType

    void display() const override {
        Task::display(); // uses parent display to show some of the info of the parent, then adds test type info
        cout << "Test Type: " << testType << '\n';
    }

    const string& getTestType() const{
        return testType;
    }
 };

class DocumentationTask : public Task {
private:
    string docType;
public:
    DocumentationTask(int taskId, const string& description, int priority, const string& status, const string& docType) 
    : Task(taskId, description, priority, status), docType(docType) {} // uses Task constructor to start, then sets up docType

    void display() const override {
        Task::display(); // uses parent display to show some of the info of the parent, then adds doc type info
        cout << "Documentation Type: " << docType << '\n';
    }

    const string& getDocType() const{
        return docType;
    }
 };


// Q3: Budgeted Interface for Multiple Inheritance [10 pts]
class Budgeted { 
protected:
    double budget;
public:
    Budgeted(double budget) : budget(budget) {}

    virtual ~Budgeted() = default;  // no dynamic memory alocation, so default is enough

    double getBudget() const {
        return budget;
    }

    void setBudget(double b){
        budget = b;
    }

    virtual void displayBudget() const = 0; 
};

// Q5: TeamMember Class [10 pts]
class BaseTeamMember {
private:
    int memberId;
    string name;
    string role;
    int maxTasks; 

public:
    BaseTeamMember(int memberId, string name, string role, int maxTasks) 
    : memberId(memberId), name(name), role(role), maxTasks(maxTasks) {} // more efficient way of doing a constructor

    virtual ~BaseTeamMember() = default; // no dynamic memory alocation, so default is enough

    BaseTeamMember(const BaseTeamMember& other) = default;

    void display() const{
        cout << "ID: " << memberId << ", Name: " 
        << name << ", Role: " << role
        << ", Max Tasks: " << maxTasks << '\n';
    }

    int getMemberId() const{
        return memberId;
    }

    const string& getName() const{
        return name;
    }

    int getMaxTasks() const{
        return maxTasks;
    }

};


// Q4: Base and Specialized Project Classes [30 pts]
class Project { 
private:
    int projectId;
    string projectName;
    Task** tasks;
    int numTasks;
    int maxTasks;
    BaseTeamMember** taskAssignments;
public:
    Project(int projectId, const string& projectName, int maxTasks) 
        : projectId(projectId), projectName(projectName), maxTasks(maxTasks), numTasks(0) {
        tasks = new Task*[maxTasks]; // array of pointers
        taskAssignments = new BaseTeamMember*[maxTasks]; 

        // make sure they are nullptr since they are pointers
        int i = 0;
        while(i < maxTasks){
            tasks[i] = nullptr;
            taskAssignments[i] = nullptr;
            i++;    
        }
    }

    virtual ~Project() {
        int i = 0;
        while(i < numTasks){
            if(tasks[i] != nullptr){ 
                delete tasks[i];
                // no need to delete taskAssignemnts because they don't belong to project. BaseTeamMember class will delete.
            }
            i++;
        }

        delete[] taskAssignments;
        delete[] tasks;
    }

    Project(const Project& other) {
        // copy the easy parts first
        projectId   = other.projectId;
        projectName = other.projectName;
        maxTasks    = other.maxTasks;
        numTasks    = other.numTasks;


        tasks = new Task*[maxTasks];
        taskAssignments = new BaseTeamMember*[maxTasks];

        // make sure they are nullptr since they are pointers
        int i = 0; 
        while(i < maxTasks){
            tasks[i] = nullptr;
            taskAssignments[i] = nullptr;
            i++;    
        }

        // copy content
        int j = 0;
        while(j < numTasks){
            if(other.tasks[j] != nullptr){
                tasks[j] = new Task(*other.tasks[j]);
            } 
    
            taskAssignments[j] = other.taskAssignments[j]; 
            j++;
        }

    }


    void addTask(Task* task) {
        // check for max, if yes, exception
        if(numTasks >= maxTasks){
            throw CapacityExceededException("max tasks reached for project");
        }

        // check if task exists, if not, exception
        if(task == nullptr){
            throw ProjectException("task is nullptr");
        }

        tasks[numTasks] = task;
        taskAssignments[numTasks] = nullptr; // no team member yet
        numTasks++;
    }

    void removeTask(int taskId) {
        
        // find task
        int i = 0;
        int indexTask = -1;
        while(i < numTasks){
            if(tasks[i] != nullptr && tasks[i]->getTaskId() == taskId){ // need to check if not nullptr
                indexTask = i;
                break;
            }
            i++;
        }
        // if didn't found task, exception
        if(indexTask == -1){
            throw TaskNotFoundException(taskId, projectId);
        }

        // delete task and move tasks to fill the removed task
        delete tasks[indexTask];
        int j = indexTask;
        while(j < numTasks - 1){
            tasks[j] = tasks[j+1];
            taskAssignments[j] = taskAssignments[j+1];
            j++;
        }

        numTasks--;
        tasks[numTasks] = nullptr;
        taskAssignments[numTasks] = nullptr;
   
    }

    Task* findTask(int taskId) {
        int i = 0;

        while(i < numTasks){
            if(nullptr != tasks[i] && tasks[i]->getTaskId() == taskId){ // need to check if not nullptr
                return tasks[i];
            }
            i++;
        }

        // if task not found, return nullptr
        return nullptr;
    }

    void assignTaskToMember(int taskId, int memberId, BaseTeamMember** teamMembers, int numMembers) {
        int i = 0;

        // find task index
        int indexTask = -1;
        while(i < numTasks){
            if(nullptr != tasks[i] && tasks[i]->getTaskId() == taskId){ // need to check if not nullptr
                indexTask = i;
                break;
            }
            i++;
        }
        // if didn't found task, exception
        if(indexTask == -1){
            throw TaskNotFoundException(taskId, projectId);
        }

        // check if task already assigned, if yes, exception
        if(taskAssignments[indexTask] != nullptr){
            throw AssignmentException("task is already assigned to a member");
        }

        // then, we find member 
        int k = 0;
        int indexMember = -1;
        while(k < numMembers){
            if(teamMembers[k] != nullptr && teamMembers[k]->getMemberId() == memberId){
                indexMember = k;
                break;
            }
            k++;
        }
        // if didn't found member, exception.
        if(indexMember == -1){
            throw MemberNotFoundException(memberId);
        }

        // assign task to member
        taskAssignments[indexTask] = teamMembers[indexMember];
        tasks[indexTask]->markAssigned();
        
    }

    BaseTeamMember* getAssignedMember(int taskId) {
        int i = 0;
        int indexTask = -1;
        while(i < numTasks){
            if(nullptr != tasks[i] && tasks[i]->getTaskId() == taskId){ // need to check if not nullptr
                indexTask = i;
                break;
            }
            i++;
        }
        
        // if task not found, exception
        if(indexTask == -1){
            throw TaskNotFoundException(taskId, projectId);
        }

        return taskAssignments[indexTask];
    }

    virtual void display() const {
        cout << "Project ID: " << projectId << "\n";
        cout << "Project Name: " << projectName << "\n";

        int i = 0;
        while(i < numTasks){
            if(tasks[i] != nullptr){
                // first display the task info
                tasks[i]->display();

                // check if task has a member assigned to it or nah
                if(taskAssignments[i] != nullptr){
                    cout << "Assigned to member ID: " << taskAssignments[i]->getMemberId() << "\n";
                } else {
                    cout << "Assigned to member ID: None\n";
                }
            }
            i++;
        }
    }

    int getProjectId() const{
        return projectId;
    }

    const string& getProjectName() const{
        return projectName; 
    }

    int getNumTasks() const{
        return numTasks;
    }

    // extra helper methods to get task by status and get tasks for members
    void displayTasksByStatus(const string& status) const {
        int i = 0;
        while(i < numTasks){
            if(tasks[i] != nullptr && tasks[i]->getStatus() == status){
                tasks[i]->display();
            }
            i++;
        }
    }

    void displayTasksForMember(BaseTeamMember* member) const {
        int i = 0;
        while(i < numTasks){
            BaseTeamMember* assignedMember = taskAssignments[i];
            if(assignedMember != nullptr && assignedMember == member && tasks[i] != nullptr){
                tasks[i]->display();
            }
            i++;
        }
    }


};

class SoftwareProject : public Project, public Budgeted { 
private:
    string technologyStack; 

public:
    SoftwareProject(int projectId, const string& projectName, int maxTasks, double budget, const string& technologyStack) 
        : Project(projectId, projectName, maxTasks), Budgeted(budget), technologyStack(technologyStack) {} // uses parent contructors 

    // replace display from parents   
    void displayBudget() const override {
        cout << "Budget: " << getBudget() << " dollars" << "\n";
    }
    
    void display() const override {
        Project::display(); // call parent display 
        cout << "Technology Stack: " << technologyStack << "\n";
        displayBudget(); 
    }

    // gets technology stack.
    const string& getTechnologyStack() const{
        return technologyStack; 
    }
};


class ResearchProject : public Project, public Budgeted { 
private:
    string researchArea;

public:
    ResearchProject(int projectId, const string& projectName, int maxTasks, double budget, const string& researchArea) 
        : Project(projectId, projectName, maxTasks), Budgeted(budget), researchArea(researchArea) {} // uses parent contructors 

     // replace display from parents   
    void displayBudget() const override {
        cout << "Budget: " << getBudget() << " dollars" << "\n";
    }
    
    void display() const override {
        Project::display(); // call parent display 
        cout << "Research Area: " << researchArea << "\n";
        displayBudget(); 
    }

    // gets research area.
    const string& getResearchArea() const{
        return researchArea; 
    }
};



// Q6: ProjectManager with Exception Handling [10 pts]
class ProjectManager {
private:
    // removed tasks and task Assignmeents since they are in project class
    // also removed projectids, project name since they are in project class. 
    Project** projects;
    int numProjects;
    int maxProjects;
    BaseTeamMember** teamMembers;
    int numMembers;
    int maxMembers;

public:
    ProjectManager(int maxProjects, int maxMembers) 
    : maxProjects(maxProjects), maxMembers(maxMembers), 
      numProjects(0), numMembers(0){
        // the counters set to  0 since we have no projects, tasks or team members
        // set arrays
        teamMembers  = new BaseTeamMember*[maxMembers];
        projects     = new Project*[maxProjects];

        // initialize projects and team member pointers to nullptr
        for(int i = 0; i < maxProjects; i++){
            projects[i] = nullptr;
        }

        for(int i = 0; i < maxMembers; i++){
            teamMembers[i] = nullptr;
        }
    }
        
    ~ProjectManager() {
        // to delete array of pointers, delete pointers inside, then delete array
        for(int i = 0; i < numProjects; i++){
            if(projects[i] != nullptr){
                delete projects[i];
            }
        }

        for(int i = 0; i < numMembers; i++){
            if(teamMembers[i] != nullptr){
                delete teamMembers[i];
            }
        }

        delete[] projects;
        delete[] teamMembers;
    }   

    // make copy constructor
    ProjectManager(const ProjectManager& other) {
        numProjects = other.numProjects;
        maxProjects = other.maxProjects;
        numMembers  = other.numMembers;
        maxMembers  = other.maxMembers;

        // for the arrays, need to create arrays then copy content. 
        teamMembers     = new BaseTeamMember*[maxMembers];
        projects        = new Project*[maxProjects];

        // initialize project and team member pointers to nullptr
        for(int i = 0; i < maxProjects; i++){
            projects[i] = nullptr;
        }
        for(int i = 0; i < maxMembers; i++){
            teamMembers[i] = nullptr;
        }

        // copy contents into them.
        for(int i= 0; i < numMembers; i++) {
            if(other.teamMembers[i] != nullptr){ 
                teamMembers[i] = new BaseTeamMember(*other.teamMembers[i]);
            }
        }    
        for(int i = 0; i < numProjects; i++){
            if(other.projects[i] != nullptr){
                projects[i] = new Project(*other.projects[i]);
            }
        }
    }


    void createProject(Project* project) {
        if(numProjects >= maxProjects){
            throw CapacityExceededException("max projects reached");
            return;
        }

        projects[numProjects] = project;
        numProjects++;
    }

    void addTeamMember(BaseTeamMember* member) {
        if(numMembers >= maxMembers){
            throw CapacityExceededException("max team members reached");
            return;
        }

        teamMembers[numMembers] = member;
        numMembers++;
    }

    // removed createTask since tasks are now created in project

    // adapted from the bool version of assignement 2.
    Project* findProject(int projectId) {
        // find project
        int i = 0;
        int indexProject = -1;
        while(i < numProjects){
            if(projects[i]->getProjectId() == projectId){
                indexProject = i;
                break;
            }
            i++;
        }

        // if no found, returns nullptr
        if(indexProject == -1){
            return nullptr;
        }

        return projects[indexProject];
    }

    BaseTeamMember* findMember(int memberId) {
        int i = 0;
        while(i < numMembers){
            int currentTeamMemberId = teamMembers[i]->getMemberId();
            if(currentTeamMemberId == memberId){
                return teamMembers[i]; // found member
            }
            i++;
        }

        return nullptr;
    }

    void assignTaskToMember(int projectId, int taskId, int memberId)  {
        // find project
        Project* currentProject = findProject(projectId);
        if(currentProject == nullptr){
            throw InvalidProjectException(projectId);
        }

        // find task to check it exists
        Task* currentTask = currentProject->findTask(taskId);
        if(currentTask == nullptr){
            throw TaskNotFoundException(taskId, projectId);
        }

        // find member to check it exists
        BaseTeamMember* currentMember = findMember(memberId);
        if(currentMember == nullptr){
            throw MemberNotFoundException(memberId);
        }

        // check if task already is assigned to a member
        BaseTeamMember* assignedMember = currentProject->getAssignedMember(taskId);
        if(assignedMember != nullptr){
            throw AssignmentException("task is already assigned to a member");
        }

        // assign task to member in project using project method
        currentProject->assignTaskToMember(taskId, memberId, teamMembers, numMembers);

    }

    BaseTeamMember* getAssignedMember(int projectId, int taskId) {
        // find project 
        Project* currentProject = findProject(projectId);
        if(currentProject == nullptr){
            throw InvalidProjectException(projectId);
        }

        // find task to check it exists
        Task* currentTask = currentProject->findTask(taskId);
        if(currentTask == nullptr){
            throw TaskNotFoundException(taskId, projectId);
        }

        // get assigned member 
        return currentProject->getAssignedMember(taskId);    
    }

    
   // removed getTasksForMember since no responsibility of project manager
   // same for getTasksByStatus

    void displayAllProjects() {
        int i = 0;
        // display projects
        while(i < numProjects){
            if(projects[i] != nullptr){
                Project* currentProject = projects[i];
                currentProject->display(); // use display method of project
            }
            i++;
        }
    }

    void displayTeamMembers() {
        int i = 0;

        // display team members
        while(i < numMembers){
            BaseTeamMember* currentMember = teamMembers[i];
            currentMember->display(); // use display method of BaseTeamMember
            
            // display their tasks
            int j = 0;
            while(j < numProjects){
                Project* currentProject = projects[j];
                if(currentProject != nullptr){
                    currentProject->displayTasksForMember(currentMember);
                }
                j++;
            }
            i++;
        }   
    }

    void displayTasksByStatus(int projectId, const string& status) {
        int i = 0;

        // find project
        Project* currentProject = findProject(projectId);
        if(currentProject == nullptr){
            throw InvalidProjectException(projectId);
        }

        // use helper method in project
        currentProject->displayTasksByStatus(status);
    }

};


int main() {
    try {
        ProjectManager manager(5, 10); // Max 5 projects, 10 team members
        // Add team members
        manager.addTeamMember(new BaseTeamMember(101, "Alice Chen", "Developer", 5));
        manager.addTeamMember(new BaseTeamMember(102, "Bob Wilson", "Tester", 3));
        manager.addTeamMember(new BaseTeamMember(103, "Carol Davis", "Manager", 10));
        // Create specialized projects (with budgets)
        manager.createProject(new SoftwareProject(1, "Website Redesign", 10, 50000.0, "Web"));
        manager.createProject(new ResearchProject(2, "AI Research", 15, 100000.0, "AI"));
        manager.createProject(new SoftwareProject(3, "Mobile App", 20, 75000.0, "Mobile"));
        // Create specialized tasks in projects
        Project* proj1 = manager.findProject(1);
        if (proj1) {
            proj1->addTask(new DevelopmentTask(1001, "Implement login system", 1, "To Do", "C++"));
            proj1->addTask(new TestingTask(1002, "Test authentication", 2, "In Progress", "Unit"));
            proj1->addTask(new DocumentationTask(1003, "Write API docs", 3, "To Do", "API"));
        }
        Project* proj2 = manager.findProject(2);
        if (proj2) {
            proj2->addTask(new DevelopmentTask(2001, "Build ML model", 1, "In Progress", "Python"));
            proj2->addTask(new TestingTask(2002, "Validate model accuracy", 2, "To Do", "Integration"));
        }
        // Assign tasks to team members
        manager.assignTaskToMember(1, 1001, 101); // Assign development task to Alice
        manager.assignTaskToMember(1, 1002, 102); // Assign testing task to Bob
        manager.assignTaskToMember(2, 2001, 101); // Assign ML task to Alice
        // Display system state
        std::cout << "=== All Projects ===" << std::endl;
        manager.displayAllProjects();
        std::cout << "\n=== Team Members ===" << std::endl;
        manager.displayTeamMembers();

        // Demonstrate exception handling
        std::cout << "\n=== Testing Exception Handling ===" << std::endl;
        try {
            manager.assignTaskToMember(999, 1001, 101); // Invalid project
        } catch (const InvalidProjectException& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }
        try {
            manager.assignTaskToMember(1, 9999, 101); // Invalid task
        } catch (const TaskNotFoundException& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }
        try {
            manager.assignTaskToMember(1, 1001, 999); // Invalid member
        } catch (const MemberNotFoundException& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }
        } catch (const std::exception& e) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
            return 1;
    }

    return 0;
}
