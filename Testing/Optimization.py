import pandas as pd
from sklearn.ensemble import RandomForestRegressor
from pymoo.core.problem import ElementwiseProblem
import numpy as np
from pymoo.algorithms.moo.nsga2 import NSGA2
from pymoo.termination import get_termination
from pymoo.optimize import minimize
from pymoo.visualization.scatter import Scatter

def import_data():
    # Load CSV without headers
    df = pd.read_csv("./results.csv", header=None)

    # Assign column names manually
    df.columns = ['x1', 'x2', 'x3', 'y1', 'y2', 'y3']

    # Split into features (independent variables) and targets (dependent variables)
    X = df[['x1', 'x2', 'x3']].values  # Features (shape should be [1210, 3])
    y1 = df['y1'].values  # Target 1 (shape should be [1210,])
    y2 = df['y2'].values  # Target 2 (shape should be [1210,])
    y3 = df['y3'].values  # Target 3 (shape should be [1210,])

    # Check the data shape
    print(X.shape)  # Should print (1210, 3)
    print(y1.shape)  # Should print (1210,)
    print(y2.shape)  # Should print (1210,)
    print(y3.shape)  # Should print (1210,)
    return X, y1, y2, y3

def to_models(Xs, Y1, Y2, Y3):
    
    model_y1 = RandomForestRegressor().fit(Xs, Y1)
    model_y2 = RandomForestRegressor().fit(Xs, Y2)
    model_y3 = RandomForestRegressor().fit(Xs, Y3)
    return model_y1, model_y2, model_y3

class MyOptimizationProblem(ElementwiseProblem):
    def __init__(self):
        super().__init__(n_var=3, n_obj=3, n_constr=0, xl=0.0, xu=1.0)  # adjust bounds as needed
        self.model_y1, self.model_y2, self.model_y3 = to_models(*import_data())


    def _evaluate(self, x, out, *args, **kwargs):
        x = np.array(x).reshape(1, -1)
        f1 = self.model_y1.predict(x)[0]
        f2 = self.model_y2.predict(x)[0]
        f3 = -self.model_y3.predict(x)[0]  # Flip sign to maximize
        out["F"] = [f1, f2, f3]


if __name__ == '__main__':
    problem = MyOptimizationProblem()
    algorithm = NSGA2(pop_size=100)
    termination = get_termination("n_gen", 100)

    res = minimize(problem,
                algorithm,
                termination,
                seed=1,
                save_history=True,
                verbose=True)

    # Plot the Pareto front
    print(f'The best input parameters: {res.X}')
    #The best input parameters: [[3.77619938e-01 5.00761321e-01 3.19060727e-03]
    Scatter(title="Pareto Front (Min y1, y2 / Max y3)").add(res.F).show()
