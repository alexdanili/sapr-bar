#include "processor.h"

Processor::Processor()
{
	n = 10;
}

void Processor::clear()
{
	aMat.clear();
	dMat.clear();
	uMat.clear();
	sMat.clear();
	nMat.clear();
	fMat.clear();
    udopMat.clear();
}


void Processor::setNodes(std::vector<float> _nodes)
{
	this->nodes = _nodes;
	clear();
}

void Processor::setBars(std::vector<std::vector<float>> _bars)
{
	this->bars = _bars;
	clear();
}

void Processor::setForces(std::vector<std::vector<float>> _forces)
{
	this->forces = _forces;
	clear();
}

void Processor::setFixators(bool l, bool r)
{
	this->leftFixator = l;
	this->rightFixator = r;
}

void Processor::calculate()
{

	aMat.resize(nodes.size());
	for(int i = 0; i < aMat.size(); i++)
	{
		aMat[i].resize(nodes.size());
	}

	for(auto item : aMat)
	{
		for(auto item2 : item)
		{
			item2 = 0;
		}
	}

	for(int i = 0; i < bars.size(); i++)
	{
		aMat[i][i] += (bars[i][3]*bars[i][2])/(nodes[bars[i][1]] - nodes[bars[i][0]]);
		aMat[i+1][i+1] += (bars[i][3]*bars[i][2])/(nodes[bars[i][1]] - nodes[bars[i][0]]);
		aMat[i][i+1] -= (bars[i][3]*bars[i][2])/(nodes[bars[i][1]] - nodes[bars[i][0]]);
		aMat[i+1][i] -= (bars[i][3]*bars[i][2])/(nodes[bars[i][1]] - nodes[bars[i][0]]);
	}

	dMat.resize(nodes.size());
	for(int i = 0; i < dMat.size();i++)
	{
		dMat[i] = -1;
	}

	if(leftFixator)
	{
		dMat[0] = 0;
	}
	if(rightFixator)
	{
		dMat[dMat.size()-1] = 0;
	}
	fMat.resize(nodes.size());
	for(int i = 0; i < nodes.size(); i++)
	{
		for(int j = 0; j < bars.size(); j++)
		{
			if(bars[j][0] == i || bars[j][1] == i)
			{
				fMat[i] += getForce(2,j)*(nodes[bars[j][1]] - nodes[bars[j][0]])/2;
			}
		}
		fMat[i] += getForce(1,i);
	}
	for(int i = 0; i < dMat.size(); i++)
	{
		if(dMat[i] == 0)
        {
            for(int j = 0; j < aMat[i].size(); j++)
			{
				aMat[i][j] = 0;
			}
            for(int j = 0; j < aMat.size(); j++)
			{
				aMat[j][i] = 0;
			}
			aMat[i][i] = 1;
			fMat[i] = 0;
		}
	}

	std::vector<std::vector<float>> tmp;
	tmp.resize(aMat.size());
	for(int i = 0; i < tmp.size(); i++)
	{
		tmp[i].resize(aMat.size()+1);
	}

	for(int i = 0; i < aMat.size(); i++)
	{
		for(int j = 0; j < aMat[i].size(); j++)
		{
			tmp[i][j] = aMat[i][j];
		}
		tmp[i][tmp.size()] = fMat[i];
	}
	dMat.clear();
	for(int i = 0; i < tmp.size(); i++)
	{
		for(int j = 0; j < tmp[i].size(); j++)
		{
			std::cout<<tmp[i][j]<<" ";
		}
		std::cout<<std::endl;
	}

	for(int i = 0; i < tmp.size(); i++)
	{
		if(i==0 && leftFixator || i==tmp.size()-1 && rightFixator)
		{
			tmp.erase(tmp.begin()+i);
			for(int j = 0; j < tmp.size(); j++)
			{
				tmp[j].erase(tmp[j].begin()+i);
			}
		}
	}
	for(int i = 0; i < tmp.size(); i++)
	{
		for(int j = 0; j < tmp[i].size(); j++)
		{
			std::cout<<tmp[i][j]<<" ";
		}
		std::cout<<std::endl;
	}


	if(leftFixator)
	{
		dMat.push_back(0);
	}
	std::vector<float> resh = kramer(tmp);
	for(auto i : resh)
	{
		dMat.push_back(i);
	}
	if(rightFixator)
	{
		dMat.push_back(0);
	}

	for(int i = 0; i < dMat.size(); i++)
	{
		std::cout<<"d"<<i+1<<"="<<dMat[i]<<std::endl;
	}
	uMat.clear();
    for(int i = 0; i < nodes.size()*2-1; i++)
	{
        uMat.push_back(dMat[(i+1)/2]);
	}
    udopMat.clear();
    for(int i = 0; i < bars.size(); i++)
    {
        udopMat.resize(bars.size());
        for(float j = 0; j < n+1; j+= 1.0f)
        {
            float u0 = uMat[i*2];
            float ul = uMat[i*2+1];
            float l = nodes[bars[i][1]]-nodes[bars[i][0]];
            float f = getForce(2,i);
            float a = bars[i][2];
            float e = bars[i][3];
			float jl = (j*(l/n));
			udopMat[i].push_back(u0 + (jl/l)*(ul-u0)+((f*l*l)/(2*a*e))*(jl/l)*(1-(jl/l)));
        }
    }
	std::cout<<"\n";
	for(int i = 0; i < udopMat.size(); i++)
	{
		for(int j = 0; j < udopMat[i].size(); j++)
		{
			std::cout<<"Ux"<<i+1<<"="<<udopMat[i][j]<<std::endl;
		}
	}
	nMat.clear();
	for(int i = 0; i < bars.size(); i++)
	{
		float po = uMat[i*2+1] - uMat[i*2];
		for(int j = 0; j <= n; j++)
		{
			nMat.push_back((bars[i][3]*bars[i][2]/(nodes[bars[i][1]] - nodes[bars[i][0]]))*(po) + (getForce(2,i)*(nodes[bars[i][1]] - nodes[bars[i][0]])/2)*(1-2*(j*(nodes[bars[i][1]] - nodes[bars[i][0]])/(n))/(nodes[bars[i][1]] - nodes[bars[i][0]])));
		}

	}
	std::cout<<"\n";

	float k=0;
	for(int i = 0; i < nMat.size(); i++)
	{

		std::cout<<"Nx"<<(i/(int)(n+1))+1<<"="<<nMat[i]<<std::endl;

	}

	sMat.clear();
	for(int i = 0; i < nMat.size(); i++)
	{
		sMat.push_back(nMat[i]/bars[i/(n+1)][2]);
	}

	std::cout<<"\n";
	for(int i = 0; i < sMat.size(); i++)
	{
		std::cout<<"Sx"<<(i/2)+1<<","<<(((i+1)%2 != 0)?"0":"L")<<"="<<sMat[i]<<std::endl;
	}

}

float Processor::getForce(float type, float point)
{
	for(int i = 0; i < forces.size(); i++)
	{
		if(forces[i][0] == type && forces[i][1] == point)
		{
			return forces[i][2];
		}
	}
	return 0;
}

std::vector<float> Processor::kramer(std::vector<std::vector<float>> A) {
	int n = A.size();
	bool empty = true;
	while(empty)
	{
		for(int i = 0; i < n; i++)
		{
			int c = 0;
			for(int j = 0; j < n; j++)
			{
				if(A[i][j] == 0)
				{
					c++;
				}
			}
			if(c == n)
			{
				A.erase(A.begin()+i);
				for(int j = 0; j < A.size(); j++)
				{
					A[j].erase(A[j].begin()+i);
				}
				n--;
			}
			else
			{
				empty = false;
			}
		}
	}
	std::vector<float> delta;
	delta.resize(n+1);
	std::vector<std::vector<float>> tmp = A;
	for(int j = 0; j < tmp.size(); j++)
	{
		tmp[j].erase(tmp[j].begin()+ tmp.size());
	}
	delta[0] = det(tmp);
	for(int i = 1; i < n+1; i++)
	{
		std::vector<std::vector<float>> tmp1 = A;
		for(int j = 0; j < tmp1.size(); j++)
		{
			tmp1[j].erase(tmp1[j].begin()+tmp1.size());
		}
		for(int j = 0; j < tmp1.size(); j++)
		{
			tmp1[j][i-1] = A[j][A.size()];
		}
		float p = det(tmp1);;
		delta[i] = p;
	}
	std::vector<float> x;
	x.resize(n);
	for(int i = 0; i < x.size(); i++)
	{
		x[i] = delta[i+1]/delta[0];
	}

	return x;
}

std::vector<std::vector<float> > Processor::getMat(std::vector<std::vector<float> > A, int _i, int _j)
{
	A.erase(A.begin()+_i);
	for(int j = 0; j < A.size(); j++)
	{
		A[j].erase(A[j].begin()+_j);
	}
	return A;
}

float Processor::det(std::vector<std::vector<float> > mat)
{
	float res = 0;
	if(mat.size() == 2)
	{
		res += mat[0][0] * mat[1][1] - mat[1][0]*mat[0][1];
	}
	else if(mat.size() == 1)
	{
		res+= mat[0][0];
	}
	else
	{
		for(int i = 0; i < mat.size(); i++)
		{
			std::vector<std::vector<float> > tmp  = mat;
			tmp.erase(tmp.begin() + 0);
			for(int j = 0; j < tmp.size(); j++)
			{
				tmp[j].erase(tmp[j].begin()+i);
			}
			res += (((i+1)%2 == 0)?1:-1)*mat[0][i]*det(tmp);
		}
	}
	return res;
}


