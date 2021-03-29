//
// Created by pmerk on 22/03/2021.
//

#ifndef ENV_HPP
#define ENV_HPP

namespace CGIenv {

	class env {
	private:
		char **_envp;

	public:
		env();
		~env();

		char **getEnv() const;
		void setEnv(char **envp);
	};

} // namespace CGIenv

#endif //ENV_HPP
