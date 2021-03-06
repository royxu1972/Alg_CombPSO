/*
** Conventional PSO
*/
#include"PSO_CPSO.h"
#include<iostream>

// ----------------------------------------------------------------------------
// 设置及初始化
// ----------------------------------------------------------------------------
void PSO_CPSO::SetConfig( int par , int ite , double w , double f )
{
	config.population = par ;
	config.iteration = ite ;
	config.weight = w ;
	config.factor = f ;
}

// ----------------------------------------------------------------------------
// 生成一条测试用例
// ----------------------------------------------------------------------------
int* PSO_CPSO::Evolve()
{
	// 返回值
	int *best = new int[sut->parameter] ;  
	
	vector<Particle> T ; 	               // 粒子群
	int *gBest = new int[sut->parameter];  // 种群最优
	int fitbest = 0 ;

	// 初始化粒子群
	for( int i = 0 ; i < config.population ; i++ )
	{
		Particle a( sut->parameter , sut->value ) ;
		a.RandomInit();

		T.push_back(a);
	}

	// gBest = T[0]
	vector<Particle>::iterator x = T.begin();     
	for( int c = 0 ; c < sut->parameter ; c++)
		gBest[c] = (*x).position[c] ;

	// 迭代次数
	int it = 1 ;

	// inertia 0.9 -> 0.4
	bool adap_inertia = false ;
	if ( config.weight == -1 )
	{
		adap_inertia = true ;
		config.weight = 0.9 ;
	}

	// 生成一个测试用例，gBest
	while( true )
	{
		// 计算每个粒子的fitness值，并更新pbest，gbest
		for( vector<Particle>::iterator i = T.begin() ; i != T.end() ; i++ )
		{
			int fit = sut->FitnessValue( (*i).position , 0 ) ;

			// 若fitness(t) = coverMax ， 返回
			if( fit == sut->testcaseCoverMax )   	
			{
				for( int c = 0 ; c< sut->parameter ; c++)
					best[c] = (*i).position[c] ;

				delete[] gBest ;
				for( vector<Particle>::iterator i = T.begin() ; i != T.end() ; i++ )
					i->clear();
				T.clear();

				return best ;
			}

			// 更新pBest
			if ( fit > (*i).fitness_pbest )
				(*i).Setpbest( fit );
			
			// 更新gBest
			if ( fit > fitbest )    
			{
				fitbest = fit ;
				for( int c = 0 ; c < sut->parameter ; c++)
					gBest[c] = (*i).position[c] ;
			}
		}  // end for

		// 中止条件
		if ( it >= config.iteration )
			break ;

		// 更新粒子群
		for( vector<Particle>::iterator i = T.begin() ; i != T.end() ; i++ )  
		{
			(*i).velocityUpdate( config.weight , config.factor , gBest );
			(*i).positionUpdate();
		} 

		// iteration++
		it++ ;

		// inertia 0.9 -> 0.4
		if( adap_inertia )
		{
			config.weight = 0.9 - 0.5 * ( (double)it / (double)config.iteration );
			//cout<<config.weight<<endl;
		}

	}  // end while

	for( int k = 0 ; k < sut->parameter ; k++ )   // best = gBest.position
		best[k] = gBest[k] ;

	delete[] gBest ;
	for( vector<Particle>::iterator i = T.begin() ; i != T.end() ; i++ )
		i->clear();
	T.clear();

	return best ;
}