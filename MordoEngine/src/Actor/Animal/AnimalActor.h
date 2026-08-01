#pragma once
#include "../Actor.h"

/// A creature: a model with a skeleton and a handful of clips it switches
/// between.
///
/// It decides nothing yet. Something else sets the state, and this turns that
/// into the right clip on the right model. That split is the point: when the AI
/// arrives it only has to call SetState, and when the same creature needs to
/// take damage it only has to gain a Health member. Neither has to learn how
/// the animation system is wired.
class AnimalActor : public Actor
{
public:
	/// Clip names exactly as the file spells them. Leave one empty when the
	/// model does not have it, and the matching state will be ignored.
	struct Clips
	{
		std::string idle;
		std::string walk;
		std::string run;
	};

	enum class State { Idle, Walking, Running };

	explicit AnimalActor(Clips clips);

	/// Switches to the clip for that state.
	/// @return false if the model has no clip for it, leaving whatever is
	///         playing alone. Set the model first: with none attached there is
	///         nothing to play on.
	bool SetState(State state);

	State GetState() const { return m_State; }

	/// Prodding it moves it up a gear and wraps back round to idle.
	///
	/// A placeholder for whatever reacting turns out to mean, and enough to
	/// prove the whole path works: the click lands on this animal and only this
	/// one changes.
	bool Interact(Actor& source, ActorContext& context) override;

private:
	Clips m_Clips;
	State m_State = State::Idle;

	/// @return The clip name for that state, or an empty string when the model
	///         was not given one.
	const std::string& ClipFor(State state) const;
};
